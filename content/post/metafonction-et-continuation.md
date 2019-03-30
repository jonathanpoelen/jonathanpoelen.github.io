---
title: "Méta-fonction et continuation"
#thumbnail: ""
#description: ""
date: 2018-11-08T18:56:13+01:00
#lastmod: 2018-11-05T21:56:13+01:00
slug: "meta-fonction-et-continuation"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2028
---

J'ai pas mal bossé avec des bibliothèques de méta-programmation, et une que j'apprécie particulièrement est [Kvasir.Mpl](https://github.com/kvasir-io/mpl). Son originalité réside dans le support des [continuations](https://fr.wikipedia.org/wiki/Continuation) et --parce que cela va bien de paire,-- l'évaluation paresseuse ainsi que des algorithmes pensés pour manipuler des packs (template variadique) plutôt que des listes de type (`list<Ts...>`).

## Continuation

Chaque algorithme dispose d'un paramètre qui décrit l'étape suivante du traitement, c'est la continuation. En shell ou dans des bibliothèques comme [rangev3](https://github.com/ericniebler/range-v3), les continuations se font avec l'opérateur `|`. Par exemple `grep x | wc -l` ou `filter(is_odd) | take(6)`. Il n'est pas possible d'utiliser `|` avec des templates alors la continuation est un paramètre ajouté à l'algorithme. En chaînant les continuations, on construit un nouvel algorithme qui s'utilise avec `call<algo, xs...>`.

{{<fhi "metafunc_continuation/decay.cpp" "proto add_const/lvalue">}}

Les continuations limitent drastiquement le recours aux "lambdas". Dans d'autres bibliothèques du genre, `add_const_lvalue_reference` s'écrirait `lambda<add_lvalue_reference, lambda<add_const, _>>` ce qui est beaucoup moins glamour.

Dans des algorithmes du type transformation l'impact sur la lecture du code est immédiat:

```cpp
transform<lambda<next, lambda<times, _1, _2>>, L1, L2>
// vs
call<transform<times<next<>, /*C=listify*/>, L1, L2>
```

Qui se traduit par la construction d'une nouvelle liste avec la formule `(x*y)+1`.

Ce code met bien en évidence un autre aspect des continuations: la lecture du code se fait de gauche à droite alors que les lambdas se lisent de droite à gauche et oblige le lecteur à faire des allers/retours pour suivre le flux de code.

```cpp
transform<lambda<next, lambda<times, _1, _2>>, L1, L2>
 /* 1 */       /* 3 */       /* 2 */
call<transform<times<next<>, /*C=listify*/>, L1, L2>
      /* 1 *//* 2 *//* 3 */      /* 4 */
```

## Méta-fonction

Au niveau de kvasir, une méta-fonction est un type qui possède un membre template nommé `f`. Tous les algorithmes de kvasir sont des méta-fonctions et l'appel se fait avec `call<metafunc, param1, param2,...>`. Si on complète `add_const` et `add_lvalue_reference` plus haut, cela donne:

{{<fhi "metafunc_continuation/decay.cpp" "impl add_const/lvalue">}}

Il ne reste qu'une dernière étape, la continuation finale lorsqu'il n'y a plus rien à faire. Les plus utilisés sont au nombre de 2 et servent de valeur par défaut à l'ensemble des algorithmes, j'ai nommé `identity` et `listify`.

{{<fhi "metafunc_continuation/decay.cpp" "f_term">}}

## C'est beau, c'est propre

Il y a quelques jours, après mon petit passage quotidien dans libstdc++ et en regardant `std::decay`, je me suis dit ~~c'est beau, c'est propre~~ qu'il faudrait comparer avec une implémentation qui utilise les concepts de kvasir.

Voici en l'état la version fournie avec gcc-8.2:

```cpp
// Decay trait for arrays and functions, used for perfect forwarding
// in make_pair, make_tuple, etc.
template<typename _Up,
     bool _IsArray = is_array<_Up>::value,
     bool _IsFunction = is_function<_Up>::value>
  struct __decay_selector;

// NB: DR 705.
template<typename _Up>
  struct __decay_selector<_Up, false, false>
  { typedef typename remove_cv<_Up>::type __type; };

template<typename _Up>
  struct __decay_selector<_Up, true, false>
  { typedef typename remove_extent<_Up>::type* __type; };

template<typename _Up>
  struct __decay_selector<_Up, false, true>
  { typedef typename add_pointer<_Up>::type __type; };

/// decay
template<typename _Tp>
  class decay
  {
    typedef typename remove_reference<_Tp>::type __remove_type;

  public:
    typedef typename __decay_selector<__remove_type>::__type type;
  };
```

En réalité, ce qui m'a particulièrement tiqué se situe dans la déclaration de `__decay_selector`: `is_array` et `is_function` sont toujours évaluées avec `_Up` et il faut écrire plusieurs spécialisations.

L'idée du sélecteur est un bon concept et se remplace facilement par une suite de conditions:

```cpp
// ce que fait plus ou moins libc++ de llvm
template<class T>
using __decay_selector = conditional<
  is_array_v<T>,
  remove_extent_t<T>*,
  conditional_t<
    is_function_v<T>,
    add_pointer_t<T>,
    remove_cv_t<T>
  >
>;
```

Qui engendre son lot de problèmes: en plus des 2 traits précédents, `remove_extent`, `add_pointer` et `remove_cv` sont toujours déclarés avec `T`. Cela ne pose pas de réel problème ici -- à part le temps de compilation--, malheureusement, dans certaines situations les branches ne doivent s'évaluer qu'en fonction du prédicat et les traits de la STL ne le permettent pas sans ajouter des intermédiaires.

Mais les continuations changent tout puisque les méta-fonctions ne sont évaluées que pour la branche qui respecte le prédicat. Ainsi, `remove_extent<T>` ne sera pas instancié lorsque `is_array_v<T>` est faux ce qui rend les continuations très facilement composables.

{{<fhi "metafunc_continuation/decay.cpp" "decay">}}

Avec l'implémentation de `if_`:

{{<fhi "metafunc_continuation/decay.cpp" "if">}}

L'ensemble des sources se trouve sur {{<urlhi "metafunc_continuation/decay.cpp" "github">}}.
