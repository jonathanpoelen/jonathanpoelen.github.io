---
title: "Implémentation d'un magasin de type"
#description: ""
date: 2015-07-02T01:49:41+01:00
#lastmod: 2018-02-13T03:40:41+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

Ce que j'appelle ici un magasin de type n'est autre qu'un `std::tuple` où les types ne sont présents qu'une seule fois. Une espèce de set version tuple en somme.

Je me suis servi de ce type de structure à 2 reprises.

Une fois pour manipuler de façon similaire des types hétérogènes sans la lourdeur de `std::tuple`. Il faut dire aussi que j'étais en C++11 et que dans cette norme `std::get<Type>()` n'existe pas.

L'autre fois dans une fonction variadique qui distribue les valeurs vers différentes fonctions. Le but étant de ne pas se soucier de l'ordre des paramètres, certains étant optionnels.

`std::tuple` fait plutôt bien le boulot, mais possède un énorme inconvénients pour ce cas de figure: aucune erreur de compilation si un type est présent 2 fois (et c'est normal pour un tuple).

## Planter la compilation quand un type est en doublon

Le C++ dispose déjà d'un mécanisme interne qui vérifie et hurle au scandale si un type doublon existe. J'ai nommé l'**héritage**.

Seulement, un héritage direct n'est pas possible avec les types scalaires, il faut un intermédiaire.

```cpp
template<class T> struct item { T x; };

template<class... Ts> struct typesets : item<Ts>... {};
```

Avec cette implémentation, des petits malins pourraient faire de la pseudo-duplication de type en y ajoutant des qualificatifs, {{<hi cpp "typesets<int, int const>"/>}} par exemple.

On peut être tolérant ou devenir un tyran sans pitié en empêchant cela.

```cpp
template<class... Ts>
struct tyrannical_typesets_impl : typesets<std::remove_cv_t<Ts>...> {
  using type = typesets<Ts...>;
};

template<class... Ts>
using tyrannical_typesets = typename tyrannical_typesets_impl<Ts...>::type;
```

Le typesets tyrannique est construit en 2 étapes, car un alias direct sur un typesets épuré ne permet pas de garder les qualificatifs.


## Piocher dans le magasin

Piquer un élément du typesets est une affaire de cast. Un simple `static_cast`.

```cpp
typesets<int, char> my_typesets;

static_cast<item<int>&>(my_typesets).x;
```

En mettant des opérateurs de cast dans la classe item, plus besoin de préciser cette dernière avec le `static_cast`.

```cpp
template<class T> struct item {
  explicit operator T& () noexcept { return x; }
  explicit operator T const& () const noexcept { return x; }
private:
  T x_;
};
```
```cpp
typesets<int, char> my_typesets;
static_cast<int&>(my_typesets);
```

Petit bémol toutefois, cela ne permet pas d'enlever l'ambiguïté pour un type qui diffère uniquement par son qualificatif.

```cpp
typesets<int, int volatile> my_typesets;

// ‘typesets<int, volatile int>’ to ‘volatile int&’ is ambiguous
static_cast<int volatile&>(my_typesets);
```

## Ce qu'il manque

- Les constructeurs, évidemment.
- Une fonction `get<Type>()` pour un parallèle avec la STL.
- Une fonction pour boucler sur chaque item (`for_each` ?).
- Et sûrement d'autres.

J'ai mis tout ça dans un repo au nom provisoire [falcon.store](https://github.com/jonathanpoelen/falcon.store).
