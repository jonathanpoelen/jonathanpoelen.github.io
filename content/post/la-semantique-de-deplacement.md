---
title: "La sémantique de déplacement"
#thumbnail: ""
#description: ""
date: 2021-05-11T22:14:00+01:00
#lastmod: 2020-12-24T00:22:00+01:00
slug: "la-semantique-de-deplacement"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2038
---

L'objectif derrière la sémantique de déplacement est de transférer les données d'un objet `A` à un objet `B`. Si les 2 objets sont du même type, on parle de constructeur de déplacement ou affectation par déplacement. Cela permet 2 choses:

- Garantir l'unicité d'une ressource. La responsabilité étant passée à quelqu'un d'autre, il n'y a toujours qu'un seul propriétaire en charge de la durée de vie de celle-ci.
- Éviter des copies profondes (deep copies) en les remplaçant par des copies superficielles (shallow copies) plus performantes.

Toute autre raison est une erreur.


## Principe d'unicité

Prenons un petit animal sauvage et nommons-le Pikachu. Ce Pikachu est unique, il n'en existe qu'un seul dans tout l'univers. Si on compare notre Pikachu à un autre Pikachu, ils sont différents, il n'y en a pas 2 pareils, même s'ils ont le même nom.

Rangeons-le dans sa pokéball.

```cpp
std::vector<Pokemon> my_bag;
my_bag.emplace_back("Pikachu");
```

Un soir, au coin du feu, un brigand passe par là et prend notre sac.

```cpp
brigand.bag = my_bag;
```

Et tout l'univers est sans dessus-dessous, notre Pikachu existe en double, le principe d'unicité est brisé !

Heureusement, `Pokemon` n'étant pas copiable, le code ne compile pas. Ouf, l'univers est sauf !

Du coup, plutôt que copier le sac, on le déplace directement dans celui de brigand en utilisant la fonction `std::move`.

```cpp
brigand.bag = std::move(my_bag);
```

Au passage, on vient d'écraser tout ce qu'il y avait dans le sac de notre voleur ; bien fait pour lui ! Mais le plus important est là: Pikachu appartient maintenant au brigand, `my_bag` ne devrait plus être utilisé. On a bien eu un transfert des pokémons d'un sac `A` vers un sac `B`, il y a eu déplacement.


## Copie profonde et copie superficielle

La copie profonde est une copie de tous les membres, y compris des données référencées par un pointeur lorsque leur durée de vie est gérée par la classe. Ce dernier point est important, car sans pointeur -- et pour aller plus loin, sans ressource, -- il n'y a pas de différence entre une copie classique ou une copie superficielle. Vouloir les opérateurs de déplacement dans cette dernière situation **ne sert à rien**, l'implémentation serait strictement identique à celle d'une copie. Autre point, même s'il y a un pointeur, il faut que les fonctions de copie fassent une copie profonde pour que les fonctions de déplacements puissent faire une copie superficielle, sinon, rebelote, aucune différence avec la copie.

Comme une illustration est plus parlante, supposons une classe `vector` avec 2 variables membres:

- `int* p`, un pointeur alloué dynamiquement et désalloué dans le destructeur
- `size_t n` qui représente le nombre d'éléments alloué

L'instance de référence nommé `A` contient les nombres 7, 1, 3, 7, 0, 5, ce qui donne en mémoire

```cpp
A = {
  p = /*adresse=*/0x12345678 /*valeurs=*/{ 7, 1, 3, 7, 0, 5 }
  n = 6
}
```

La copie profonde va faire une nouvelle allocation dynamique et copier les valeurs de `A.p` dans `B.p`. L'adresse du pointeur est donc différente, mais le contenu est identique.

```cpp
// B = A
B = {
  p = /*adresse=*/0x87654321 /*valeurs=*/{ 7, 1, 3, 7, 0, 5 }
  n = 6
}
```

La copie superficielle effectuée par un déplacement n'alloue pas de mémoire, elle copie simplement `A.p` dans `B.p` qui est une opération bien plus rapide. L'adresse des pointeurs est identique.

```cpp
// B = std::move(A)
B = {
  p = /*adresse=*/0x12345678 /*valeurs=*/{ 7, 1, 3, 7, 0, 5 }
  n = 6
}
```

Malheureusement, lorsque le destructeur est appelé, il libère la mémoire du pointeur `p` qui est partagé entre `A` et `B`. Cela donne inévitablement une double désallocation qui finit sur un crash de l'application. Pour prévenir cette erreur, `A.p` ne doit pas être libéré, par exemple en mettant le pointeur à `nullptr`.

```cpp
// B = std::move(A)
A = {
  p = nullptr
  n = 0
}
```

Le déplacement a le même fonctionnement que le principe d'unicité: l'allocation dynamique (la ressource) ne doit être possédée que par une seule instance.



## Constructeur de déplacement

Le constructeur de déplacement prend ce qu'on nomme une rvalue (noté `T&&`). C'est une référence qui se veut temporaire. Si la valeur de cette expression provient d'une opération, elle doit être capturée dans la classe, autrement, elle est perdue. Lorsque cette valeur provient d'un déplacement explicite comme avec `std::move(x)`, il faut considérer que `x` est dans un état qui ne permet plus de l'utiliser (sauf si la documentation indique le contraire).

À savoir que toutes variables -- quel que soit son type réel -- **est toujours** manipulée comme une lvalue. C’est-à-dire qu'avec `int i; foo(i);`, la fonction `foo()` reçoit une référence (`int&`), pas juste `int`.

Pour prendre un exemple connu, les chapitres suivants reposent sur le fonctionnement de `std::unique_ptr`, un pointeur intelligent qui fait une désallocation automatique de la mémoire dans son destructeur et interdit la copie pour respecter le principe d'unicité.


```cpp
#include <cassert>

struct unique_ptr
{
  using value_type = int; // normalement un type template,
                          // mais pour cet exemple, juste un int

  unique_ptr(value_type* p = nullptr)
  : m_p(p)
  {}

  // Notre constructeur de déplacement
  // D'après la documentation std::unique_ptr, après cette fonction
  // other.m_p doit être nullptr
  unique_ptr(unique_ptr&& other);

  ~unique_ptr()
  {
    delete m_p;
  }

  // Pour simplifier, la classe ne possède que `operator*` et `operator bool ()`

  explicit operator bool () const
  {
    return m_p;
  }

  value_type& operator*()
  {
    assert(m_p);
    return *m_p;
  }

private:
  value_type* m_p;
};
```

Et un premier exemple d'utilisation.

```cpp
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

Reste l'implémentation du constructeur de déplacement. Comme dit précédemment, seule une instance doit posséder le pointeur interne. L'instance déplacée doit être modifiée pour ne plus y faire référence, tout en restant dans un état dit **destructible** pour que le destructeur fonctionne convenablement. Les prérequis de [MoveConstructible](https://en.cppreference.com/w/cpp/named_req/MoveConstructible) parlent d'un état non spécifié. C'est-à-dire que l'implémentation est libre de faire ce qu'elle veut du moment que la destruction fonctionne encore, mais il ne faut plus utiliser la variable.

Cependant, chaque fonction peut explicitement documenter le comportement comme c'est le cas avec `std::unique_ptr` qui met le pointeur déplacé à `nullptr`.

```cpp
unique_ptr::unique_ptr(unique_ptr&& other)
: _p(std::exchange(other._p, nullptr))
{}
```

Finalement beaucoup d'explications pour 1 ligne de code. Mais nous sommes loin d'avoir terminé, notre `unique_ptr` ne respecte pas tous les prérequis nécessaires pour un bon constructeur de déplacement. Il n'y a pas non plus d'affectation par déplacement qui amène à de grosse surprise. Et surtout, qui nous dit qu'il n'est pas copiable ?



## Fonctions spéciales

Une classe possède 6 fonctions spéciales générées automatiquement par le compilateur:

- le constructeur par défaut
- le constructeur par copie
- l'affectation par copie
- le constructeur par déplacement
- l'affectation par déplacement
- le destructeur

Si aucune de ces fonctions n'est déclarée dans la classe, leur existence dépend des membres la composant. Ainsi, si un membre comme `std::unique_ptr` existe, les 2 fonctions liées à la copie seront implicitement supprimées car inexistantes pour le type `std::unique_ptr`.

De plus, définir explicitement certaines fonctions va en désactiver d'autres. Il est nécessaire d'utiliser `=default` pour les réactiver.

<sub>déclare</sub> / <sup>existe</sup> | default-ctor | copy-ctor | copy-assignment | move-ctor | move-assignment
-------------------------------------- | ------------ | --------- | --------------- | --------- | ---------------
default-constructor                    |       ✓      |     ✓     |        ✓        |     ✓     |        ✓
copy-constructor                       |              |     ✓     |        ✓        |           |
copy-assignment                        |       ✓      |     ✓     |        ✓        |           |
move-constructor                       |              |           |                 |     ✓     |
move-assignment                        |       ✓      |           |                 |           |        ✓
destructor                             |       ✓      |     ✓     |        ✓        |           |

À cela s'ajoute que le constructeur par défaut n'est plus défini en présence de n'importe quel autre constructeur (pas uniquement ceux de copie ou de déplacement).

Si on reprend notre `unique_ptr` précédemment, ce tableau affirme une chose: la copie n'est pas possible et l'affectation par déplacement est bien manquante.

À titre personnel, je pense qu'il vaut mieux explicitement indiquer que la copie est interdite, soit via une classe spécifique comme `boost::noncopyable` soit en ajoutant les prototypes suivants:

```cpp
unique_ptr(unique_ptr const&) = delete;
unique_ptr& operator=(unique_ptr const&) = delete;
```

Quitte à déclarer certaines fonctions comme étant supprimées, il est aussi plus explicite pour l'utilisateur de la classe de mettre explicitement `=default` pour les autres fonctions. C'est le principe de la [règle de 5](https://en.cppreference.com/w/cpp/language/rule_of_three#Rule_of_five) qui consiste à définir explicitement les fonctions spéciales (en excluant le constructeur par défaut dans cette règle).

À savoir aussi que -- sauf cas très spécifique -- les constructeurs et `operator=` vont par paire. Si l'un est implémenté, l'autre devrait l'être également. Ce que nous allons faire dans le prochain chapitre.



## Affectation par déplacement

Cette fonction est proche du constructeur de déplacement, mais possède un petit piège qu'il est bon de savoir. Commençons par une implémentation possible:

```cpp
unique_ptr& operator=(unique_ptr&& other)
{
  delete m_p;
  m_p = std::exchange(other.m_p, nullptr); // ligne qu'on retrouve dans le constructeur
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
  print(p2); // p2 contient int* sur 3
  print(p1); // p1 est maintenant nullptr
}
```

Mais que se passe-t-il si on fait un déplacement sur soi-même ?

```cpp
int main()
{
  unique_ptr p{new int(3)};
  print(p); // 3
  p = std::move(p);
  print(p); // ???
} // double-free !!!
```

Le code affiche n'importe quoi et explose ! La raison est toute bête, on désalloue le pointeur puis on récupère celui dans `other` qui est identique au pointeur précédemment libéré. Une écriture aussi explicite que `p = std::move(p)` n'a pas beaucoup de sens, mais il est possible d'arriver dans une telle situation avec un code plus complexe et 2 variables à priori bien distinctes.


Si on se réfère au prérequis de [MoveAssignable](https://en.cppreference.com/w/cpp/named_req/MoveAssignable), il n'y a aucune indication sur l'état de `t` dans `t = rv` lorsque `t` et `rv` sont la même référence. Plusieurs choix s'offrent à nous en cas de self-move-assignment:

- considérer cela comme un comportement indéfini
- définir `rv` comme étant égal à nul (donc le pointeur est ici supprimé)
- définir `t` comme contenant le pointeur de `rv` (et donc ici ne rien faire)


### Self-move-assignment comme comportement indéfini

Ce choix peut paraitre étrange voire dangereux, mais il est justifié pour un besoin de performance: le déplacement doit être rapide. Gérer un tel scénario demande du code supplémentaire -- généralement un `if (this != &other)` -- et cela peut avoir un impact signification pour un cas de figure fortement marginal. Le choix du standard penche beaucoup pour un comportement indéfini et seules certaines classes l'autorisent.

Pour information, les implémentations de libc++ et libstdc++ (clang et gcc) vident les containers tel que `std::vector`. Il y a même une assertion si on utilise libstdc++ avec la macro `_GLIBCXX_DEBUG`.

```cpp
#include <vector>

int main()
{
  std::vector<int> v{1,2,3};
  v = std::move(v); // kaboum avec -D_GLIBCXX_DEBUG: https://godbolt.org/z/Gn4KWe
  // v.size() == 0
}
```

```sh
g++ test.cpp -D_GLIBCXX_DEBUG && ./a.out
[...]
Error: attempt to self move assign.
[...]
```


### Définir l'état de `rv` sur self-move-assignment

Dans ce scénario, seul l'état de rv dans `t = rv` est défini comme étant à nul. Pour ce faire, on désalloue le pointeur de `t` puis on le met à `nullptr`. Après un déplacement sur soi-même, le pointeur est systématiquement détruit.

```cpp
unique_ptr& operator=(unique_ptr&& other)
{
  delete m_p;
  m_p = nullptr; // other.m_p devient nullptr quand other.m_p == m_p
  m_p = std::exchange(other.m_p, nullptr);
  return *this;
}
```

Ce comportement n'est pas très logique. Ce qui importe dans un déplacement est de connaître l'état de la destination (`t`). De plus, se retrouver avec `t == nullptr` est en contradiction avec le comportement principal du déplacement: « la valeur de `t` est équivalent à la valeur de `rv` avant affectation ».

Finalement ce choix n'est pas très judicieux.


### Définir l'état de `t` sur self-move-assignment

Ici l'état de `t` est défini, même quand `t = rv` équivaut à un déplacement sur soi-même. Si c'est le cas, on ne désalloue rien et le déplacement ne fait rien: on a bien `t` équivalent à la valeur de `rv` avant affectation.

```cpp
unique_ptr& operator=(unique_ptr&& other)
{
  auto* new_p = std::exchange(other.m_p, nullptr);
  delete m_p; // m_p = nullptr quand other.m_p == m_p
  m_p = new_p;
  return *this;
}
```

Cette dernière peut aussi s'écrire avec l'idiome [copy-and-swap](https://en.cppreference.com/w/cpp/language/operators#Assignment_operator) ou avec `if (this == &other) return *this` en début de fonction pour complètement ignorer l'affectation sur soi-même. Personnellement, j'évite les conditions dans les fonctions de déplacement lorsque cela est possible.

```cpp
// copy-and-swap
unique_ptr& operator=(unique_ptr other) // on prend par valeur
{
  using std::swap;
  swap(other.m_p, m_p); // et on swap
  return *this;
  // le destructeur de other va libérer la mémoire à sa destruction
}
```

Ce comportement pour le déplacement est celui documenté dans `std::unique_ptr` qui impose l'équivalence à `reset(r.release())`: la valeur ne change pas lorsque `t` et `rv` référencent le même objet.


## Les déplacements devraient être `noexcept`


C'est une chose qu'on oublie facilement, mais les fonctions de déplacement devraient être `noexcept` pour 2 raisons simples:

- Le déplacement est une opération qui se veut la plus triviale possible. Les risques d'exception sont normalement nuls.
- Les containers de la STL utilisent les fonctions de déplacement à la condition que ceux-ci sont `noexcept` ou qu'il n'y ait pas de fonction de copie. Voici un exemple qui montre le problème.

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

// copie autorisée, mais déplacement qui n'est pas noexcept
struct B
{
  B()=default;
  B(B const&)=default;
  B(B&&) { std::puts("B&&"); }
};

// copie autorisée et déplacement noexcept
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

(Je n'ai pas mis `operator=`, mais il est évident que la version par déplacement devrait aussi être noexcept.)

Une note sur l'implémentation derrière: les containers se basent sur la fonction [std::move_if_noexcept](https://en.cppreference.com/w/cpp/utility/move_if_noexcept).



## La vérité vraie du mensonge qu'est std::move

Je ne vais pas mentir, tout le baratin précédent n'est là que pour placer ce chapitre. Autant de bla bla juste pour le plaisir de mettre ce titre :).

Ceci dit, arrivé ici, vous devriez être conscient que `std::move` ne fait pas grand-chose: tout se situe dans les constructeurs et opérateurs de déplacement.

Mais alors, que fait `std::move` ? Eh bien, rien... Ou plus précisément, la fonction ne touche pas à l'instance, mais à la catégorie de valeur. Ce n'est rien de plus qu'un cast d'une lvalue en rvalue ! On pourrait tout aussi bien remplacer `std::move(x)` par `static_cast<std::remove_reference_t<decltype(x)>&&>(x) `, le résultat serait exactement le même -- à la verbosité près.

Puisque `std::move` n'est rien de plus qu'un cast, il est inutile de l'utiliser sur des rvalues. `std::move(Bidule{})` n'a aucun sens, `Bidule{}` est déjà une rvalue (mieux, c'est une prvalue). Il n'est pas non plus nécessaire de l'utiliser sur le retour des fonctions qui bénéficient de la [RVO](https://cpp.developpez.com/faq/cpp/?page=Optimisation#Qu-est-ce-que-la-RVO) et de l'élision de copie en général.

Pire, utiliser `std::move` lorsque cela n'est pas nécessaire désactive certaines optimisations. Sur ce point, gcc et clang ont tous deux les avertissements `-Wpessimizing-move` et `-Wredundant-move` qui se déclenchent sur une utilisation inappropriée de `std::move`.

```cpp
A foo()
{
  A ret;
  // ...
  return std::move(ret); // -Wpessimizing-move
  return ret; // NRVO
}

A foo(A a)
{
  return std::move(a); // -Wredundant-move
}

A bar()
{
  foo(std::move(A{})); // -Wpessimizing-move
  return std::move(foo()); // -Wpessimizing-move
  return foo(); // RVO
}
```


## std::forward

Pour finaliser les explications sur le déplacement, il faut introduire `std::forward`. 

Cette fonction n'est utile que sur des types templates dont la catégorie de valeur n'est pas connue. L'exemple le plus simple est une fonction `template<class T> void foo(T&& x);` où `T` représente une forwarding reference. Càd une référence qui est soit une lvalue, soit une rvalue. On peut aussi croiser le nom de référence universelle venant d'avant la normalisation du nom officiel.

Il faut bien comprendre que les forwarding references s'appliquent sur un type template complet, ce qui n'est pas le cas par exemple pour `void foo(std::vector<T>&& vec)` où la fonction attend toujours une rvalue.

Le but de `std::forward` est de propager la référence en castant une variable vers une rvalue quand le type d'origine est une rvalue (n'oublions pas qu'à ce niveau, une variable est une lvalue, même si son type est une rvalue).

```cpp
#include <iostream>

void foo(int&& x) { std::cout << "foo(int&&)\n"; }
void foo(int& x) { std::cout << "foo(int&)\n"; }

template<class T>
void bar(T&& x)
{
  foo(std::forward<T>(x)); // `x` est castée en rvalue lorsque T&& est une rvalue
}

int main()
{
  int i = 0;
  bar(i);            // foo(int&)
  bar(std::move(i)); // foo(int&&)
}
```

Sans l'usage de `std::forward`, les 2 appels donneraient `foo(int&)`.

Si on veut comprendre la magie derrière, il faut regarder le type réel de `T`:

- `bar(i)`: `T` = `int&`. `T&&` = `int&`
- `bar(std::move(i))`: `T` = `int`. `T&&` = `int`

Appliquer une rvalue sur un type qui est une lvalue donne une lvalue. C'est ce qu'on appel les règles de [reference collapsing](https://en.cppreference.com/w/cpp/language/reference#Reference_collapsing).

 lhs  | rhs  | référence
------|------|-----------
 `&`  | `&`  | `&`
 `&`  | `&&` | `&`
 `&&` | `&`  | `&`
 `&&` | `&&` | `&&`

C'est également le mécanisme derrière `std::forward<T>(x)` qui combine simplement `T` à une rvalue pour caster la variable dans la bonne catégorie de valeur. Ceci est strictement équivalent à `static_cast<T&&>(x)` ou `static_cast<decltype(x)&&>(x)`. Certains projets définissent une macro `FWD(x)` qui fonctionne ainsi.



## Que personne ne bouge, v'là la conclusion

Pour résumer tout ça:

- `std::move` n'est qu'un cast user-friendly vers une rvalue, ce n'est pas lui qui fait le déplacement à proprement parler. Mal l'utiliser désactive aussi certaines optimisations.
- Le comportement du déplacement est défini par les fonctions qui reçoivent une rvalue.
- Définir certaines fonctions spéciales en désactivent d'autres, il est préférable d'indiquer explicitement le comportement de chacune de préférence avec `=default` ou `=delete`. Pour rappel, les fonctions spéciales sont ici les constructeurs de déplacement et de copie, l'affectation par déplacement et de copie ainsi que le destructeur.
- le constructeur de déplacement et l'affectation par déplacement devrait être noexcept pour que les containers de la STL les utilisent.
- `std::forward` s'utilise pour des paramètres template de la forme `T&&` pour propager le type de référence (lvalue ou rvalue).

Voilà qui clôture cet article sur la sémantique de déplacement. Et n'oubliez pas, une variable n'est jamais une rvalue et -- sauf exception de la NRVO -- il faut explicitement utiliser `std::move` pour l'utiliser comme une rvalue.
