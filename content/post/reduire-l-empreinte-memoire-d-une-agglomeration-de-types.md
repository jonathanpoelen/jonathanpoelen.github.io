---
title: "Réduire l'empreinte mémoire d'une agglomération de types"
slug: "reduire-lempreinte-memoire-dune-agglomeration-de-types"
#description: ""
date: 2014-06-28T14:10:11+01:00
#lastmod: 2018-02-13T03:37:11+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2048
---

Un petit article pour parler d'optimisation mémoire (si on peut appeler ça comme ça) avec comme exemple la structure de donnée utilisée par [std::unique_ptr](http://en.cppreference.com/w/cpp/memory/unique_ptr).

## Implémentation naïve de std::unique_ptr

Pour rappel, `std::unique_ptr` prend 2 paramètres template: `T` et `Deleter` (qui par défaut égal `std::default_delete<T>`).

Naïvement, l'implémentation serait:

```cpp
template<T, Deleter = std::default_delete<T>>
class my_unique_ptr {
  T* m_pointer;
  Deleter m_deleter;
  // …
};
```

Rien d'extraordinaire.

Cependant, même si `Deleter` est une classe sans attribut, sa taille est de 1 octet.

À partir d'ici je considère que `Deleter` est toujours la valeur par défaut, ce qui donne:

- {{<hi cpp "sizeof(T*)"/>}} == 8
- {{<hi cpp "sizeof(Deleter)"/>}} == 1
- {{<hi cpp "sizeof(my_unique_ptr<T>)"/>}} == 16
- {{<hi cpp "sizeof(std::unique_ptr<T>)"/>}} == 8

Ouille, méchant padding, alors que seuls 8 octets sont vraiment utilisés.

## Comment fait la STL pour "supprimer" 8 octets ?

La bibliohèque standard utilise une optimisation surnommée [Empty Base Class Optimization (EBCO)](http://en.cppreference.com/w/cpp/language/ebo).
Concrètement, cela se traduit par une classe interne qui contient le pointeur et hérite de `Deleter`. Les attributs de la classe dérivée vont se mettre après ceux de `Deleter`, et s'il n'en a pas, ils se positionnent au début de la classe. Grâce à cette astuce, l'adresse du premier membre de la classe (ici, le pointeur) se confond avec celle de la classe englobante et parente, éliminant ainsi l'espace occupé par `Deleter`.

```cpp
template<T, Deleter = std::default_delete<T>>
class my_unique_ptr {
  struct internal : Deleter {
    T* pointeur;
  } m_data;
  // ...
};
```

- {{<hi cpp "sizeof(my_unique_ptr<T>)"/>}} == 8

Mieux, non ?

## Et si l'héritage n'est pas possible ?

Si le Deleter est une référence ou une classe {{<hi cpp "final"/>}}, l'héritage ne fonctionne pas. Il faut se rabattre sur la première forme (celle naïve).
Avec des traits et un code plus ou moins volumineux, cela est "facile".
Il faut cependant noter que {{<hi cpp "std::is_final"/>}} n'apparaît qu'à partir de C++14 et son implémentation n'est pas possible en pure C++.
Il faut à la place utiliser `__is_final` qui n'est pas standard.

Toutefois, la STL possède un conteneur générique qui utilise l'EBO si possible: {{<hi cpp "std::tuple"/>}}. Ce qui permet de s'affranchir de ces difficultés tout en optimisant l'espace mémoire à condition de mettre les types dans l'ordre croissant d'alignement pour réduire le padding entre les membres lorsqu'il y en a plus de 2.
