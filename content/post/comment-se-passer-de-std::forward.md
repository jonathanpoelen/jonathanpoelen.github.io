---
title: "Comment se passer de std::forward"
#description: ""
date: 2016-04-04T23:09:41+01:00
#lastmod: 2018-02-13T03:46:41+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

Actuellement en pleine écriture d'une fonction `match` (petit projet de 200 lignes actuellement), je me retrouve, pour respecter le perfect forwarding, avec une armée de `std::forward` utilisée à chaque appel de fonction. J'en compte 21 pour un total de 6 niveaux d'imbrications. Autant dire qu'oublier de le mettre est plutôt facile.

C'est d'ailleurs la raison de ce billet, puisque bien sûr, j'en ai oubliés.

Pour réduire leur nombre et alléger le code, je me suis fait un petit wrapper qui sauvegarde le type de référence. Comme l'objet créé a pleinement connaissance du type de variable qu'il contient et que traverser une chaîne de fonctions ne fait pas disparaître cette information, il n'y a plus besoin de std::forward. Le dernier maillon peut alors extraire une lvalue ou une rvalue via --par exemple-- une fonction `get()`.

Pour illustrer, voilà ce que cela donne:

```cpp
template<class T>
struct forwarder
{
  T & x;
  T && get() const { return static_cast<T&&>(x); }
};

template<class T>
struct forwarder<T&>
{
  T & x;
  T & get() const { return x; }
};


#include <iostream>

template<class T> void print(T &&) { std::cout << "&&\n"; }
template<class T> void print(T &) { std::cout << "&\n"; }

// dernier niveau d'imbrication avant l'appel de la vrai fonction
// note: le coup d'une copie est nulle pour le type forwarder
template<class T> void f1(T x) { print(x.get()); }

// pas besoin de std::forward
template<class T> void f2(T x) { f1(x); }

// premier maillon, création du contexte
template<class T> void foo(T && x) { f2(forwarder<T>{x}); }

int main()
{
  int i = 1;
  int const ci = 2;

  foo(i); // &
  foo(ci); // &
  foo(3); // &&
  foo(std::move(i)); // &&
  foo(std::move(ci)); // &&
}
```

Avec 2 fonctions d'accès par référence en plus, `ref` et `cref` pour la version constante, on possède une petite classe qui simplifie pas mal la vie.
