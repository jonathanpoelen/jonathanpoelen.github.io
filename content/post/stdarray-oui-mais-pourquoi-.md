---
title: "std::array, oui, mais pourquoi ?"
#thumbnail: ""
#description: ""
date: 2019-03-29T17:28:19+01:00
#lastmod: 2019-03-29T17:28:19+01:00
slug: "stdarray-oui-mais-pourquoi-"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2028
---

Depuis C++11, un nouveau type de tableau fait son apparition: `std::array`. S'il est là, ce n'est pas uniquement parce que la STL est cool, mais bien parce que les tableaux C posent des problèmes dans lesquels les débutants sautent à pieds joints.


## Les tableaux C se convertissent en pointeur trop facilement

Le tableau C a l'alarmante faculté de se convertir en pointeur par simple affectation ou opération arithmétique. Par exemple, Soustraire 2 tableaux donne la distance qui sépare les 2 variables dans la mémoire, ce qui n'a aucun sens. Mais puisque les tableaux se dégradent en pointeur, le compilateur l'accepte sans broncher.

Le seul pseudo-avantage est l'arithmétique des pointeurs qui permet de manipuler un tableau presque comme un pointeur -- à la différence que l'incrémentation et la décrémentation ne sont pas possibles.

Ainsi, on pourra écrire `auto* p = a + i` plutôt que `auto* p = &a[i]`.

Ou encore `&i[a]` (i.e. `&2[a]`), forme uniquement valide avec des tableaux ou des pointeurs. Tant qu'à hériter du C, prenons le meilleur... :D


## Votre prototype de fonction ment

Voici une fonction tout à fait banale qui affiche les valeurs d'un tableau:

```cpp
void print(int const array[3])
{
  for (int i : array)
  {
    std::cout << i << "\n";
  }
}
```

Fonction qui ne compile pas, car `array` n'est pas un tableau, mais un pointeur. Et un pointeur ne fonctionne pas avec les boucles sur intervalle. Gcc indique l'erreur en affichant le prototype tel qu'il devrait être lu: "dans la fonction `void print(const int*)`". Clang va même jusqu'à dire qu'un paramètre de type `int[3]` est considéré comme un pointeur. La conversion en pointeur se propage même à ce niveau.

Ce qui veut dire qu'écrire un prototype qui prend un tableau de 3 `int` est un mensonge. Le compilateur ne fera aucune vérification sur la taille du tableau passé en paramètre. Pour lui, que l'argument soit un tableau de 1, 2, 3 ou plus d'éléments, c'est pareil: un pointeur. Par conséquent, les 4 prototypes suivants sont strictement identiques:

```cpp
void print(int const array[4]);
void print(int const array[3]);
void print(int const array[]);
void print(int const* array);
```

Bien sûr, il est possible d'avoir un vrai tableau en paramètre avec l'aide des références et d'une syntaxe alambiquée:

```cpp
void print(int const (&array)[3]); // bienvenue dans le monde merveilleux de C++
```

À ce moment, le compilateur considère `array` comme étant un tableau de 3 entiers constants et la boucle précédente pourra fonctionner. Si l'utilisateur met un tableau de moins ou de plus de 3 éléments, le compilateur va gentiment l'envoyer bouler.


## Un tableau C se convertit en entier... Dans certains circonstances

Il est possible de convertir un tableau C en entier sans faire exprès, de la même manière qu'un pointeur se convertit en entier.

```cpp
using T = unsigned long long;

void foo(T);

int a[10];

foo(T(a1)); // ok, on passe l'adresse de `a`. À ne pas confondre avec la valeur du premier élément
```

J'ai déjà eu ce genre d'erreur dans un code proche de `write(a, std::size_t(a))` à la place de `write(a, std::size(a))`.


## Les tailles des tableaux multidimensionnels sont à l'envers

Lorsqu'on déclare un tableau multidimensionnel, l'ordre des dimensions est à lire à l'envers. Ce qui n'est pas le cas en utilisant des alias.

```cpp
int a[2][3]; // tableau de 2 tableaux de 3 int

using A = int[2]; // tableau de 2 int
A a[3]; // tableau de 3 tableaux de 2 int
```


## Un tableau C n'est pas copiable

Le tableau est le seul type du C qui ne supporte ni la copie, ni l'affectation, ce qui le rend inutilisable en retour de fonction ou dans n'importe quel conteneur de la STL tel que `std::vector`. Il n'est pas non plus possible de construire un tableau directement dans l'appel d'une fonction sauf en C99 ou C++ avec un cast ou à travers un alias.

```cpp
// cast
foo((int[]){1,2});

// alias
template<class T>
using carray = T[];

foo(carray<int>{1,2});
```

Par contre, une structure qui contient un tableau est aussi bien copiable qu'affectable. Manipuler un tableau directement impose plusieurs contraintes complètement loufoques, mais mettez le tout dans une boîte et tout est permis. Ce qui m'amène à `std::array`, car il fait justement office de boîte.


## Std::array, un conteneur comme les autres

Le gros avantage de `std::array` est son interface commune avec les autres conteneurs:

- `size()`
- `empty()`
- `begin()`/`end()`
- `data()`

Ainsi que quelques membres utilitaires comme `fill()`, `front()`, `back()` et la panoplie de type comme `value_type`, `reference`, etc qu'on s'attend à voir.

Si je prends le cas de `size()`, la version tableau C est beaucoup plus compliquée: `sizeof(array)/sizeof(array[0])`, Mais aussi dangereuse, car le comportement sera totalement imprévisible si, suite à un refactoring, notre tableau est remplacé par `std::vector`.

La manière intelligente de faire consiste en une fonction libre `size(T(&)[N])` qui s'occupe de cela pour nous. Si le type change, alors la fonction ne correspond plus et des erreurs apparaissent. Au passage, C++17 introduit `std::size(cont)`, `std::empty(cont)` et `std::data(cont)` valides pour tous les conteneurs, y compris les tableaux. Voici [un article de Lmghs](https://linuxfr.org/news/cpp17-libere-size-data-et-empty) sur le sujet et les raisons de ce choix.


## Un tableau de 0 élément

Dans certaines circonstances, on peut vouloir un tableau de 0 élément. Cela fonctionne très bien avec `std::array` contrairement au tableau C qui doit utiliser une extension du compilateur pour le supporter (c'est interdit par le standard). Pour pallier à ce problème avec les tableau C, sa taille est généralement forcée à 1, mais d'autres complications surviennent dès que les types ne sont pas trivialement constructibles.


## Std::array est un tuple

Propriété anecdotique, les fonctions et classes associées au tuple sont disponibles pour `std::array`. Cela permet par exemple de jouer avec `std::apply` pour transformer un `std::array` en un pack variadique.

```cpp
std::array<int, 3> a{1,2,3};
int sum = std::apply([](auto... xs){ return xs + ...; }, a);
```

Dans la pratique, il n'y a pas d'inconvénient à le faire sur un tableau C ici, mais le standard ne le prévoit pas.


## Déduction de taille VS taille explicite

Un gros avantage du tableau C se situe sur la déclaration de la taille au moment de l'initialisation: le compilateur peut la déduire. Alors qu'avec `std::array` il faut la mettre en paramètre template au risque d'y mettre une valeur trop grande (une valeur trop petite donne une erreur).

Sauf que depuis C++17, les [guides de déduction](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction) rendent optionnels les paramètres template. Ce n'est pas un strict équivalent puisque le type est aussi déduit, mais c'est généralement ce qu'on veut car tous les éléments doivent être du même type[^1]. Dans le pire des cas, on peut se tourner vers quelque chose comme `std::make_array<T>(xs...)` (en TS) qui permet de spécifier le type du tableau sans indiquer explicitement la taille.

```cpp
int a1[3]{1,2,3}
int a2[]{1,2,3} // taille implicite

std::array<int, 3> a3{1,2,3};
std::array a4{1,2,3}; // taille et type implicites
std::array a5 = std::make_array<int>(1,2,3); // taille implicite
```

[^1]: On notera que `auto a[] {1,2,3}` n'est pas autorisé.
