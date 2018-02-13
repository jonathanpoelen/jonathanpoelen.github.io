---
title: "Minimiser les copies dans operator+"
date: 2017-07-05T22:02:11+02:00
draft: false
#tags: [ "c++" ]
categories: [ "c++" ]
ghcommentid: 1
---

Je vais me baser sur un classique: une classe de matrice contenant un `std::vector<int>`. Cette classe va implémenter 2 opérateurs mathématiques: `+` et `+=`. Le premier en fonction libre, le second en fonction membre.

Pour rigoler un peu, on ajoute une petite contrainte qui est "l'efficacité". Petit mot qui englobe un peu tout et n'importe quoi tel que la performance en mémoire et en temps.

À vrai dire, il y a énormément de choses possibles rien que sur la structure du code: instruction vectorisée, alignement mémoire, expression template, etc. Des bibliothèques comme uBLAS, Eigen, Blitz implémentent une tripotée de choses. Ici, on va uniquement s'intéresser à la manière d'implémenter `operator+` pour recycler les variables temporaires dans le but d'avoir le moins d'allocations possibles dûes aux copies.

Grosso-modo, des rvalues à droite, des rvalues à gauche, des rvalues partout et pour finir, pas de rvalue.

En réalité, il y a plusieurs approches possibles que je mets ici en opposition sans qu'elles le soient réellement.

1. une surcharge pour tous les prototypes possibles.
2. un opérator unique pour les gouverner tous.


## Plein de surcharges de operator+

Faire 4 prototypes pour distinguer les rvalues des lvalues est un choix assez naturel. Si un prototype contient une rvalue, alors il y a moyen de recycler une valeur. On pourrait même ajouter `noexcept` sur de tels prototypes.

Voici ce que donne l'implémentation:

```cpp
Matrix operator+(Matrix const& lhs, Matrix const& rhs)
{
  Matrix ret {lhs};
  ret += rhs;
  return ret; // et non pas return `ret += rhs`, ce qui empêcherait la NRVO.
}

Matrix operator+(Matrix&& lhs, Matrix const& rhs)
{
  lhs += rhs;
  return std::move(lhs); // ne pas oublier std::move, sinon il y a aura copie en sortie
}

Matrix operator+(Matrix const& lhs, Matrix&& rhs)
{
  rhs += lhs; // commutativité: x+y = y+x
  return std::move(rhs);
}

Matrix operator+(Matrix&& lhs, Matrix&& rhs)
{
  rhs += lhs; // éventuellement `rhs += std::move(lhs)`
  return std::move(rhs);
}
```

Petite note sur la dernière implémentation. Utiliser `rhs` comme valeur de retour permet de gagner un `mov` (asm). [ici][cmp_rhs_lsh_return].

Bon, c'est bien joli, mais on peut quasiment faire la même avec seulement 2 prototypes. Seulement, pour une raison que j'ignore, ni clang, ni gcc n'applique la RVO correctement. Le constructeur de déplacement est systèmatiquement utilisé.

```cpp
Matrix operator+(Matrix lhs, Matrix const& rhs)
{
  lhs += rhs;
  return lhs; // pas de RVO ???
}

Matrix operator+(Matrix const& lhs, Matrix&& rhs)
{
  rhs += lhs; // commutativité: x+y = y+x
  return std::move(rhs);
}
```

Les prototypes ne sont pas symétriques pour éviter les ambiguïtés. Le prototype prenant un paramètre par copie sera moins prioritaire que celui avec une rvalue, mais il accepte toutes les formes de référence.

Ainsi, si dans l'expresssion `a + b`, `b` une rvalue, la seconde fonction sera utilisée. Dans les autres cas, la première fonction sera utilisée. On peut facilement vérifier quelle expression correspond à quelle fonction avec un `std::cout << __PRETTY_FUNCTION__ << '\n'` dans les implémentations et le test qui suit.

```cpp
template<class Lhs, class Rhs>
void test()
{
  std::cout << "Matrix " << (std::is_const<Lhs>{} ? "const" : "     ") << " a; ";
  std::cout << "Matrix " << (std::is_const<Rhs>{} ? "const" : "     ") << " b;\n\n";

  Lhs a;
  Rhs b;

  std::cout << std::left;
  #define C(a,b) std::cout << std::setw(13) << #a << "+ " << std::setw(15) << #b; a+b
  C(a,            b);
  C(a,            std::move(b));
  C(std::move(a), b);
  C(std::move(a), std::move(b));
  #undef C
}

int main()
{
  test<      Matrix,       Matrix>(); std::cout << "\n\n";
  test<      Matrix, const Matrix>(); std::cout << "\n\n";
  test<const Matrix,       Matrix>(); std::cout << "\n\n";
  test<const Matrix, const Matrix>();
}
```

Résultat:

```
Matrix       a; Matrix       b;

a            + b              Matrix operator+(Matrix, const Matrix&)
a            + std::move(b)   Matrix operator+(const Matrix&, Matrix&&)
std::move(a) + b              Matrix operator+(Matrix, const Matrix&)
std::move(a) + std::move(b)   Matrix operator+(const Matrix&, Matrix&&)


Matrix       a; Matrix const b;

a            + b              Matrix operator+(Matrix, const Matrix&)
a            + std::move(b)   Matrix operator+(Matrix, const Matrix&)
std::move(a) + b              Matrix operator+(Matrix, const Matrix&)
std::move(a) + std::move(b)   Matrix operator+(Matrix, const Matrix&)


Matrix const a; Matrix       b;

a            + b              Matrix operator+(Matrix, const Matrix&)
a            + std::move(b)   Matrix operator+(const Matrix&, Matrix&&)
std::move(a) + b              Matrix operator+(Matrix, const Matrix&)
std::move(a) + std::move(b)   Matrix operator+(const Matrix&, Matrix&&)


Matrix const a; Matrix const b;

a            + b              Matrix operator+(Matrix, const Matrix&)
a            + std::move(b)   Matrix operator+(Matrix, const Matrix&)
std::move(a) + b              Matrix operator+(Matrix, const Matrix&)
std::move(a) + std::move(b)   Matrix operator+(Matrix, const Matrix&)
```

Si on y tient vraiment, on peut ajouter `Matrix operator+(Matrix&&, Matrix&&)`. Mais comme dit précédemment le besoin est très faible.


## Un prototype multi-fonction

Une autre solution pour la surcharge d'opérateur est de ne faire qu'un seul et unique prototype template qui s'active en présence d'un certain type. Ce n'est pas une approche opposée à la précédente (elle peut servir de complément), mais je vais présenter ici comment le faire avec seulement un prototype.

Pour filtrer les types compatibles, on va utiliser la bonne vieille méthode à base de `std::enable_if`. Ce qui donne:

```cpp
template<class MatrixLhs, class MatrixRhs>
std::enable_if_t<
  std::is_same<std::decay_t<MatrixLhs>, Matrix>::value &&
  std::is_same<std::decay_t<MatrixRhs>, Matrix>::value
  Matrix>
operator+(MatrixLhs&& lhs, MatrixRhs&& rhs);
```

Dans la réalité, l'addition d'une matrice fonctionne aussi sur des entiers (cf: `int + Matrix`, `Matrix + int`). Le filtre sera alors beaucoup plus compliqué puisqu'il faut qu'au moins une des opérandes soit un type `Matrix` et que les paramètres soient des types compatibles (en prenant en compte la préscence des références et des `const`). La condition devient alors quelque chose comme:

```cpp
is_matrix_operand<Lhs> &&
is_matrix_operand<Rhs> &&
(is_matrix<Lhs> || is_matrix<Rhs>)
```

Il devient alors très facile d'ajouter un nouveau type à prendre en compte, comme par exemple un contenaire de la SL, un tableau C, un autre type matriciel d'une autre bibliothèque, etc. Faire comme dans le premier chapitre avec un prototype pour chaque cas devient vite infernal.

Il est également envisageable de faire des prototypes par catégorie de variable: Sequence et Matrix, Integer et Matrix.

Revenons-en à notre `operator+` et son implémentation. Celle-ci va être plus compliqué car elle doit être équivalente aux 4 implémentations du début ; sachant que la première possède une variable et les opérandes sont inversés dans la troisième et la quatrième.

Une solution possible est de mettre 2 valeurs intermédiaires qui représentent l'opérande de gauche et l'opérande de droite et dont le type s'adapte en fonction des types en entrée.

Ci-dessous un tableau récapitulatif des types et valeurs de nos 2 nouvelles variables `Lhs` et `Rhs`. Les `const` sont supprimés car seule la référence importe.

   Prototype   |    NewLhs    |   NewRhs
---------------|--------------|-------------
`M & `, `M & ` | `M   ` = lhs | `M & ` = rhs
`M &&`, `M & ` | `M &&` = lhs | `M & ` = rhs
`M & `, `M &&` | `M &&` = rhs | `M & ` = lhs
`M &&`, `M &&` | `M &&` = rhs | `M &&` = lhs

Et l'implémentation:

```cpp
template<class T>
struct rvalue_wrapper
{
  T& value;

  operator T&& () const noexcept
  { return static_cast<T&&>(value); }
};

template<class T>
T&& unwrap(rvalue_wrapper<T> x) noexcept
{
  return x;
}

template<class T>
T&& unwrap(T&& x) noexcept
{
  return std::forward<T>(x);
}

template<class Lhs, class Rhs>
Matrix operator +(Lhs&& lhs, Rhs&& rhs)
{
  using NewLhs = std::conditional_t<
    std::is_lvalue_reference<Lhs>::value &&
    std::is_lvalue_reference<Rhs>::value,
    Matrix,
    rvalue_wrapper<Matrix>>;

  using NewRhs = std::conditional_t<
    !std::is_lvalue_reference<Lhs>::value &&
    !std::is_lvalue_reference<Rhs>::value,
    Matrix&,
    Matrix&&>;

  constexpr bool swap_arg = !std::is_lvalue_reference<Rhs>::value;

  NewLhs new_lhs{const_cast<Matrix&>(swap_arg ? rhs : lhs)};
  NewRhs new_rhs{static_cast<NewRhs>(const_cast<Matrix&>(swap_arg ? lhs : rhs))};

  unwrap(new_lhs) += static_cast<NewRhs>(new_rhs);

  return new_lhs;
}
```

Le code mérite quelques explications. Pour commencer, parlons de `rvalue_reference` qui est un palliatif pour une optimisation au niveau de `return`. Au niveau du retour, si `NewLhs` est une rvalue, il faut utiliser `std::move`, sauf que l'utiliser sur une variable locale à la fonction bloque le RVO. Hélas, même avec un `if (std::is_rvalue_reference<NewLhs>{}) return std::move(lhs);` avant `return new_lhs` l'optimisation n'est pas faite. Cela fonctionne néanmoins avec `if constexpr` de c++17. Le but de `rvalue_reference`  est finalement de rendre automatique un retour par rvalue grâce à l'opérateur de cast interne.

Concernant ce curieux enchaînement de cast, celui-ci s'explique par la difficulté de contrôler le type retourner par une ternaire. Une ternaire sur deux variables de même type va retourner une référence (une variable est toujours une lvalue). La référence sera considérée constante si une des deux valeurs est une référence constante. Du coup, on vire le const pour ensuite construire les types `NewLhs` et `NewRhs`.

Ici, le constructeur de la matrice (quand `NewLhs = Matrix`) va recevoir un type non const. À moins qu'un constructeur existe pour les références non const, cela ne cause pas de problème. On peut très bien ajouter un `std::conditional` pour forcer le const.

En première impression `static_cast<NewRhs>` pourrait être optionnel, mais celui-ci permet de forcer la rvalue pour construire NewRhs. Une lvalue (le retour de `const_cast<Matrix&>`) ne pouvant être affectée à une rvalue sans cela.

Les casts présents fonctionnent bien parce que `lhs` et `rhs` sont tous deux du même type. Dans le cas contraire, il faut faire un branchement à la compilation via de la surcharge de fonction (dispatch de type) tel que font `falcon::cif` ou `boost::hana::if_`. Plusieurs de mes articles en parlent.


## N'écrivez pas operator+ vous-même, c'est trop compliqué&nbsp;!

Sérieusement, qui veut écrire une 20taine de lignes pour chaque opérateur ? Ne le faites pas, le code est allourdi, la lisibilité réduite. Il y a moyen d'implémenter la plupart des opérateurs en quelques lignes pour le même résultat.

De plus, l'implémentation des opérateurs peuvent varier. Par exemple, pas de commutativité. Ses variantes sont difficiles à détecter dans une grande masse de code, il devient facile de faire une erreur aussi bien à l'écriture qu'à la lecture.

Autre point, les types des opérandes peuvent être nombreux, faire tous les prototypes necessaires vous vaudra des heures de souffrances :).

Du coup, comment faire ? Une solution facile est d'utiliser une macro pour implémenter les opérateurs voulus. C'est simple et rapide, mais l'utiliser avec des types template est un peu délicat. Cela reste néanmoins la solution la plus simple.

Une autre manière passe par du CRTP pour que la classe de base implémente les opérateurs voulus sous forme de fonction amie. C'est la solution de [boost/operators.hpp](http://www.boost.org/doc/libs/1_64_0/libs/utility/operators.htm). Malheureusement, elle ne prend pas en compte les optimisations possibles sur les rvalues écrits dans cet article. Il faut la ré-écrire.

La dernière solution consiste à se servir des traits pour activer ou non certains prototypes comme dans le chapitre précédent. Une mise en oeuvre poussée peut être extrêmement extensible et s'adapte très facilement aux catégories de valeur (séquence, intégrale), mais c'est un poil complexe à mettre en place. Je ne connais pas de bibliothèque qui le fasse.

Au final, il n'existe actuellement pas d'outil satisfaisant pour générer les opérateurs alors qu'il est presque aussi rapide d'écrire une lib ou des macros pour le faire. Le temps perdu sera largement compensé par le nombre d'opérateurs à implémenter par la suite. Avec un peu de jugeote, il est même possible de mutualiser l'écriture des opérateurs `@=`. Pensez-y la prochaine fois qu'il faudra écrire des opérateurs ;).


[cmp_rhs_lsh_return]:https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(j:1,source:'%23include+%3Cutility%3E%0A%23include+%3Cvector%3E%0A%0Astruct+Matrix%0A%7B%0A++++++std::vector%3Cint%3E+v%3B%0A++++++Matrix%26+operator+%2B%3D(Matrix+const%26)+%3B%0A%7D%3B%0A%0AMatrix+operator%2B+(Matrix+%26%26+lhs,+Matrix+const+%26+rhs)%0A%7B%0A++++std::move(lhs)+%2B%3D+rhs%3B%0A++++//+ne+pas+oublier+std::move,+sinon+il+y+a+aura+copie+en+sortie+puisque+que+lhs+est+une+r%C3%A9f%C3%A9rence%0A++++return+std::move(lhs)%3B%0A%7D%0A%0AMatrix+operator+%2B+(Matrix+const+%26+lhs,+Matrix+%26%26+rhs)%0A%7B%0A++++std::move(rhs)+%2B%3D+lhs%3B+//+commutativit%C3%A9:+x%2By+%3D+y%2Bx%0A++++return+std::move(rhs)%3B%0A%7D%0A'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:31.812073713509285,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:compiler,i:(compiler:g71,filters:(b:'0',commentOnly:'0',directives:'0',intel:'0'),options:'-O3+-std%3Dc%2B%2B14+-fverbose-asm+',source:1),l:'5',n:'0',o:'x86-64+gcc+7.1+(Editor+%231,+Compiler+%231)',t:'0')),k:34.07627444894807,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((h:compiler,i:(compiler:g71,filters:(b:'0',commentOnly:'0',directives:'0',intel:'0'),options:'+-std%3Dc%2B%2B14+-O3+-fverbose-asm+',source:1),l:'5',n:'0',o:'x86-64+gcc+7.1+(Editor+%231,+Compiler+%232)',t:'0')),k:34.11165183754265,l:'4',n:'0',o:'',s:0,t:'0')),l:'2',n:'0',o:'',t:'0')),version:4
