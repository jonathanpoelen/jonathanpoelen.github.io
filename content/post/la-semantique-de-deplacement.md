---
title: "la sémantique de déplacement"
#thumbnail: ""
#description: ""
date: 2020-12-24T22:14:00+01:00
#lastmod: 2020-12-24T00:22:00+01:00
slug: "la-semantique-de-deplacement"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: true
ghcommentid: 0
expire: 2038
---

L'objectif derrière la sémantique de déplacement est de transférer les données d'un objet A à un objet B. Si les 2 objets sont du même type, on parle de constructeur de déplacement ou affectation par déplacement. Cela permet 2 choses:

- Garantir l'unicité d'une ressource. La responsabilité étant passé à quelqu'un d'autre, il n'y a toujours qu'un seul propriétaire en charge de la durée de vie de celle-ci.
- Éviter des copies profondes en les remplaçant par des copies superficielles.

TODO Toutes autres raison est une erreur



## Principe d'unicité

Prenons une pierre et nommons-la Racaillou. Ce Racaillou est unique, il n'en n'existe qu'un seul dans tout l'univers. Si on compare notre Racaillou à un autre Racaillou, ils sont différents, il n'y en a pas 2 pareil, même s'ils ont le même nom.

Rangeons-le dans notre inventaire.

```cpp
std::vector<Item> my_inventory;
my_inventory.emplace_back("Racaillou");
```

Un soir, au coin du feu, un brigant passe par là et prend notre inventaire.

```cpp
brigand.inventory = my_inventory;
```

Et tout l'univers et sans dessus-dessous, notre Racaillou existe en double, le principe d'unicité est brisé !

Heuresement, `Item` n'étant pas copiable, le code ne compile pas. Ouf, l'univers est sauf !

Du coup, plutôt que copier l'inventaire, on le déplace directement dans celui de brigand en utlisant la fonction `std::move`.

```cpp
brigand.inventory = std::move(my_inventory);
```

Au passage, on vient d'écraser tout ce qu'il y avait dans l'inventaire de notre voleur¹, mais le plus important est là: Racaillou appartient maintenant au brigand. `my_inventory` est vide, sa taille est de 0. On a bien eu un transfer des items d'un inventaire `A` vers un inventaire `B`, il y a eu déplacement.

[1] Bien fait pour lui !


## Copie profonde et copie superficielle

La copie profonde est une copie de tous les membres, y comprit des données référencés par un pointeur. Ce dernier point est important, car sans pointeur, il n'y a pas de différence avec une copie classique ou superficelle. Vouloir les opérateurs de déplacement dans cette situation **ne sert à rien**. Autre point, même s'il y a pointeur, il faut que les fonctions de copie fassent une copie profonde pour que les fonctions de déplacements puisse faire une copie superficelle, sinon, rebelotte, aucune différence avec la copie.

Comme une illustration est plus parlante, supposons une classe `vector` avec 2 variables membres:

- `int* p`, un pointeur alloué dynamiquement et désalloué dans le destructeur
- `size_t n` qui représente le nombre d'élément alloué

(Toute ressemble avec une classe nommée `std::vector` est forfuite.)

L'instance de référence nommé `A` contient les nombres 7, 1, 3, 7, 0, 5, ce qui donne en mémoire

```cpp
A = {
  p = /*adresse=*/0x12345678 /*valeurs=*/{ 7, 1, 3, 7, 0, 5 }
  n = 6
}
```

La copie profonde va faire une nouvelle allocation dynamique et copier les valeurs de `A.p` dans `B.p`. L'addresse du pointeur est donc différente, mais le contenu est identique.

```cpp
// B = A
B = {
  p = /*adresse=*/0x87654321 /*valeurs=*/{ 7, 1, 3, 7, 0, 5 }
  n = 6
}
```

La copie superficielle effectuée par un déplacement n'allou pas de mémoire, elle copie simplement `A.p` dans `B.p` qui est une opération bien plus rapide.

```cpp
// B = std::move(A)
B = {
  p = /*adresse=*/0x12345678 /*valeurs=*/{ 7, 1, 3, 7, 0, 5 }
  n = 6
}
```

Malheuresement, lorque le destructeur est appelé, il libère la mémoire du pointeur `p` qui est partagé entre `A` et `B`. Cela donne inévitablement une double désallocation qui finit sur un crash de l'application. Pour prévenir cette erreur, `A.p` ne doit pas être libérer, par exemple en mettant le pointeur à `nullptr`.

```cpp
// B = std::move(A)
A = {
  p = nullptr
  n = 0
}
```

En réalité, même si la copie est possible, le déplacement d'une telle classe a le même fonctionnement qu'avec le principe d'unicité: l'allocation dynamique (la ressource) ne doit être possédée que par une seule instance.



## Catégorie de valeur

Tout le principe de sémantique de déplacement repose sur l'introduction des rvalues. Les rvalues font partie d'un ensemble de 5 catégories de valeur qui sont: lvalue, prvalue, xvalue, glvalue et rvalue. Ça c'est que dit la norme, en tant que développeur, il n'y a que 2 types qui sont différenciables dans un programme: lvalue et rvalue. Les autres deviennent automatiquement soit des lvalues, soit rvalues suivant le contexte. On peut les oublier.

- lvalue pour left value dans l'expression `x = y`.
- rvalue pour right value dans l'expression `x = y`.

Qui une explication fréquence, mensongère et surtout fausse. Ici, `x` et `y` sont toutes les 2 des lvalues, on ne peut pas réduire l'explication à simplement droite/gauche. Voici une explication plus juste.

- Une lvalue est une référence (noté `T&`). Toutes variables -- quelque soit son type réel -- **est toujours** une lvalue. C'est à dire qu'avec `int i; foo(i);`, la fonction `foo()` reçoit une référence (`int&`), pas juste `int`.
- Une rvalue est une expression temporaire (noté `T&&`). Si la valeur de l'expression n'est pas capturée dans une variable, elle est perdue. Mais -- et c'est sur cela que repose la sémantique de déplacement --, il est possible de convertir une référence en référence temporaire avec `std::move()`.

```cpp
int main()
{
  {
    int & lvalue = 3; // Erreur, 3 n'est pas une variable, ni une référence,
                      // mais une valeur temporaire
    int && rvalue = 3; // Ok, mais cela est dangereux à cause de l'aspect temporaire
                       // des rvalues. Il faut éviter de les converser, mais plutôt
                       // les "accrocher" à une variable
    int value = 3; // C'est.... Un point d'encrage d'un temporaire
  }

  {
    int i = 3;

    int & lvalue = i;
    int && rvalue = i; // Erreur, i n'est pas une rvalue
    int && rvalue = std::move(i);

    int && rvalue2 = rvalue; // Erreur, rvalue n'est pas une... rvalue.
                             // Rapellez-vous, toutes variables est une lvalue
    int & lvalue2 = rvalue;
    int && rvalue2 = std::move(rvalue);
  }
}
```



## Constructeur de déplacement

Pour prendre un exemple connue, le code de ce chapitre repose sur `std::unique_ptr`, un pointeur intelligent qui fait une désallocation automatique de la mémoire dans son destructeur et interdit la copie pour respecter le principe d'unicité.

Pour simplifier, la classe ne travaille qu'avec des `int` et ne possède que `operator*` et `operator bool ()` comme fonction membre.

```cpp
#include <cassert>

using T = int; // normalement un type template,
               // mais pour cet exemple, juste un int

struct unique_ptr
{
  unique_ptr(T* p = nullptr) : _p(p) {}
  ~unique_ptr() { delete _p; }

  // notre constructeur de déplacement
  unique_ptr(unique_ptr&& other);

  explicit operator bool () const
  {
    return _p;
  }

  int& operator*()
  {
    assert(_p);
    return *_p;
  }

private:
  T* _p;
};
```

```cpp
// exemple
#include <iostream>
#include <utility>

void print(unique_ptr& ptr)
{
  if (ptr) std::cout << *ptr;
  else std::cout << "nullptr";
  std::cout << "\n";
}

int main()
{
  unique_ptr p1(new int(3));
  unique_ptr p2{std::move(p1)}; // on déplace p1 dans p2

  // p1 est vide, cela affiche nullptr
  std::cout << "p1: ";
  print(p1);

  // p2 possède un pointer valide, cela affiche 3
  std::cout << "p2: ";
  print(p2);
}
```

Reste l'implémentation du constructeur de déplacement. Comme dit précédement, seule une instance possède le pointeur interne. L'instance déplacé doit être modifiée pour ne plus y faire référence, tout en restant dans un état dit **destructible** pour que le déstructeur fonctionne convenablement. Les prérequis de [MoveConstructible](https://en.cppreference.com/w/cpp/named_req/MoveConstructible) par d'un état non spécifié, mais chaque fonction peut ajouter ses propres contraintes. Le plus simple, ici, est de mettre le pointeur à `nullptr`.

```cpp
unique_ptr::unique_ptr(unique_ptr&& other)
: _p(std::exchange(other._p, nullptr))
{}
```

Finalement beaucoup d'explication pour 1 ligne de code. Mais nous somme loin d'avoir terminé, notre `unique_ptr` ne respecte pas tous les prérequis mécessaire pour un bon constructeur de déplacement. Il n'y pas non plus d'affectation par déplacement qui amène à de grosse surprise. Et surtout, qui nous dit qu'il n'est pas copiable ?


## Fonctions spéciales

Une classe possède 6 fonctions spéciales générées automatiquement par le compilateur:

- le constructeur par défaut
- le constructeur par copie
- l'affectation par copie
- le constructeur par déplacement
- l'affectation par déplacement
- le destructeur

Si aucune de ces fonctions n'est déclarée dans la classe, leur existance dépend des membres la composant. Ainsi, si un membre comme `std::unique_ptr` existe, les 2 fonctions liées à la copie seront implicitement supprimées car inexistant pour le type `std::unique_ptr`.

À l'inverse, définir explicitement certaines fonctions va en désactiver d'autres. Il est nécessaire d'utiliser `=default` pour les réactiver.

<sub>déclare</sub> / <sup>existe</sup> | default-ctor | copy-ctor | copy-assignment | move-ctor | move-assignment
-------------------------------------- | ------------ | --------- | --------------- | --------- | ---------------
default-ctor                           |       ✓      |     ✓     |        ✓        |     ✓     |        ✓
copy-ctor                              |              |     ✓     |        ✓        |           |
copy-assignment                        |       ✓      |     ✓     |        ✓        |           |
move-ctor                              |              |           |                 |     ✓     |
move-assignment                        |       ✓      |           |                 |           |        ✓
dtor                                   |       ✓      |     ✓     |        ✓        |           |

À cela s'ajoute que le constructeur par défaut n'est plus définit en présence de n'importe quel autre constructeur (pas uniquement ceux de copie ou de déplacement).

Si on reprend notre `unique_ptr` précédement, ce tableau affirme une chose: la copie n'est pas possible et l'effectation par déplacement est bien manquante.

À titre personnel, je pense qu'il vaut mieux explicitement indiquer que la copie est interdite, soit via une classe spécifique comme boost::noncopyable soit en ajoutant les prototypes suivants:

```cpp
unique_ptr(unique_ptr const&) = delete;
unique_ptr& operator=(unique_ptr const&) = delete;
```

À savoir aussi que -- sauf cas très spécifique -- les constructeurs et `operator=` vont par paire. Si l'un est implémenté l'autre devrait l'être également. Ce que nous allons faire dans le prochain chapitre.



## Affectation par déplacement

Cette fonction est proche du constructeur de déplacement, mais possède un petit piège qu'il est bon de savoir. Commençons par l'implémentation classique:

```cpp
unique_ptr& operator=(unique_ptr&& other)
{
  delete _p;
  _p = std::exchange(other._p, nullptr); // ligne qu'on retrouve dans le constructeur
  return *this;
}
```

En apparence, aucun problème, le pointeur à l'intérieur de `other` est déplacé puis remis à zéro, alors que l'ancien se fait détruire. Un petit test le confirme:

```cpp
int main()
{
  unique_ptr p1{new int(3)};
  unique_ptr p2;

  print(p2); // nullptr
  p2 = std::move(p1);
  print(p2); // p2 contient int*(3)
  print(p1); // p1 est maintenant nullptr
}
```

Mais que se passe-t-il si on fait un déplacement sur soit-même ?

```cpp
int main()
{
  unique_ptr p{new int(3)};
  print(p); // 3
  p = std::move(p);
  print(p); // ???
} // double-free !!!
```

Le code affiche n'importe quoi et explose ! La raison est toute bête, on désaloue le pointeur puis on récupère celui dans `other` qui est identique au pointeur précédement libéré. Il faut protéger la fonction contre ce genre de scénario.

Note: Une ligne aussi explicite que `p = std::move(p);` n'a pas beaucoup de sens, mais il est possible d'arriver à une telle situation avec un code plus complexe et 2 variables à prioris bien distingues.

Si on se réfère au prérequis de [MoveAssignable](https://en.cppreference.com/w/cpp/named_req/MoveAssignable), il n'y a aucune indication sur l'état de `t` dans `t = rv` lorsque `t` et `rv` sont la même référence. 2 choix possibles:

- On désalloue le pointeur puis on le met à `nullptr`. Après un déplacement sur soit-même, le pointeur est systématiquement détruit.

```cpp
unique_ptr& operator=(unique_ptr&& other)
{
  delete _p;
  _p = nullptr; // other._p devient nullptr quand other._p == _p
  _p = std::exchange(other._p, nullptr);
  return *this;
}
```

- On ne désalloue rien et le déplacement ne fait rien

```cpp
unique_ptr& operator=(unique_ptr&& other)
{
  auto* new_p = std::exchange(other._p, nullptr);
  delete _p; // _p = nullptr quand other._p == _p
  _p = new_p;
  return *this;
}
```

Cette dernière peut aussi s'écrire avec l'idiome [copy-and-swap](https://en.cppreference.com/w/cpp/language/operators#Assignment_operator) ou avec `if (this == &other) return *this` en début de fonction pour complétement ignorer l'affectation sur soit-même. À titre personel, je préfère évider les conditions dans les fonctions de déplacement lorsque cela est possible.

```cpp
unique_ptr& operator=(unique_ptr other) // on prend par valeur
{
  using std::swap;
  swap(other._p, _p); // et on swap
  return *this;
  // le destructeur de other va libérer la mémoire à sa destruction
}
```

Bien que les 2 comportements soient autorisés par la norme, le second est plus logique. Après `p = std::move(p);` on s'attend à ce que le pointeur ne change pas. Il faudra se référer à la documentation de chaque classe pour savoir quel est le comportement d'une telle affectation. Pour `std::unique_ptr`, le standard impose l'équivalence à `reset(r.release())`: la valeur ne change pas. Par contre, les containers n'ont aucun prérequis et ils seront dans un état non spécifiés. À savoir que les implémentations de libc++ et libstdc++ (clang et gcc) vide le container. Il y a même une assertion si on utilise libstdc++ avec `-D_GLIBCXX_DEBUG`.

```cpp
std::vector v{1,2};
v = std::move(v); // kaboum avec -D_GLIBCXX_DEBUG: https://godbolt.org/z/Gn4KWe
// v.size() == 0
```



## Les déplacements devrait être `noexcept`

C'est une chose qu'on oublit facilement, mais les fonctions de déplacement devrait être noexcept pour une simple raison: les containers de la STL utilisent les fonctions de déplacement à la condition que ceux si sont `noexcept` ou qu'il n'ait pas de fonction de copie. Voici un exemple montre le problème.

```cpp
#include <vector>
#include <cstdio>

// copie interdite
struct A
{
  A()=default;
  A(A const&)=delete;
  A(A&&) { std::puts("A&&"); }
};

// copie autorisé, mais déplacement qui n'est pas noexcept
struct B
{
  B()=default;
  B(B const&)=default;
  B(B&&) { std::puts("B&&"); }
};

// copie autorisé et déplacement noexcept
struct C
{
  C()=default;
  C(C const&)=default;
  C(C&&) noexcept { std::puts("C&&"); }
};

int main()
{
  // va afficher au moins un A&&
  std::vector<A> a;
  a.emplace_back();
  a.emplace_back();
  a.emplace_back();

  // aucun B&&
  std::vector<B> b;
  b.emplace_back();
  b.emplace_back();
  b.emplace_back();

  // va afficher au moins un C&&
  std::vector<C> c;
  c.emplace_back();
  c.emplace_back();
  c.emplace_back();
}
```

(Je n'ai pas mi `operator=`, mais il est évident que la version par déplacement devrait aussi être noexcept)



## La vérité vrai du mensonge qu'est std::move

Je ne vais pas mentir, tout le baradin des chapitres précédents n'est là que pour placer ce chapitre. Autant de bla bla juste pour le plaisir de mettre ce titre :).

Ceci dit, arrivé ici, vous devriez être consient que `std::move` ne fait pas grand chose: tout se situt dans les constructeur et opérateur de déplacement.

Mais alors, que fait `std::move` ? Eh bien, rien... Ou plus précisement, la fonction ne touche pas à l'instance, mais à la catégorie de valeur, ce n'est rien de plus qu'un cast d'une lvalue en rvalue. On pourrait tout aussi bien remplacé `std::move(x)` par `static_cast<std::remove_reference_t<decltype(x)>&&>(x) `, le résultat serait exactemnent le même -- à la verbosité près.

Puisque `std::move` n'est rien de plus qu'un cast, il est inutile de l'utiliser sur des rvalues. `std::move(Bidule{})` n'a aucun sens, `Bidule{}` est déjà une rvalue (mieux, c'est une prvalue). Il n'est pas non plus nécessaire de l'utiliser sur le retour des fonctions qui bénificient de la [NRVO](https://cpp.developpez.com/faq/cpp/?page=Optimisation#Qu-est-ce-que-la-RVO).

```cpp
A foo()
{
  A ret;
  // ...
  return ret; // pas besoin de std::move
}
```

Sur ce point, gcc et clang ont tous deux les avertissements `-Wpessimizing-move` et `-Wredundant-move` qui se déclenchent sur une utilisation inapropriée de `std::move`.



## std::forward

Pour finaliser les explications sur le déplacement, il faut introduire `std::forward` et les règles de [reference collapsing](https://en.cppreference.com/w/cpp/language/reference#Reference_collapsing).

`std::forward` n'est utile que sur des types templates, dont la catégorie de valeur n'est pas connue. L'exemple le plus simple est une fonction `template<class T> void foo(T&& x);` où `T` représente une forwarding reference (càd une référence qui est soit une lvalue, soit une rvalue).

Sur un usage classique de foo, le type réel de `T` est le suivant:

- `foo(std::string())`: `T` = `std::string`. `T&&` = `std::string`
- `foo(str)`: `T` = `std::string&`. `T&&` = `std::string&`

Appliquer une rvalue sur un type qui est une lvalue donne une lvalue. C'est le point essentiel du references collapsing.

 lhs  | rhs  | référence
------|------|-----------
 `&`  | `&`  | `&`
 `&`  | `&&` | `&`
 `&&` | `&`  | `&`
 `&&` | `&&` | `&&`


Le but de `std::forward` est de propager la catégorie de valeur en castant vers une rvalue quand le type d'origine est une rvalue (n'oublions pas qu'à ce niveau, la variable `x` est une lvalue, même si son type est une rvalue).

Pour ce faire, `std::forward<T>(x)` combine simplement `T` à une rvalue pour ensuite caster la variable dans la bonne catégorie de valeur. Ceci est strictement équivalent à `static_cast<T&&>(x)` ou `static_cast<decltype(x)&&>(x)`. Certains projets définissent une macro `FWD(x)` qui fonctionne ainsi.

Les explications sont complexes, mais la chose importante à retenir est qu'un type template de la forme `T&&` doit être propagé avec `std::forward<T>` pour conserver le type de référence.



# Que personne ne bouge, v'là la conclusion

Voilà qui cloture cet article sur la sémantique de déplacement. Et n'oubliez pas, une variable n'est jamais une rvalue et -- sauf exception de la NRVO -- il faut explicitement utiliser `std::move` pour en avoir une rvalue. Et avoir les constructeur et affectation de déplacement en noexcept.
