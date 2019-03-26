---
title: "Parcourir les arguments d'une fonction variadique"
#description: ""
date: 2013-12-22T10:03:06+01:00
lastmod: 2019-03-23T03:23:06+01:00
lastmod_change: "Ajout du chapitre « Fold expression et C++17 »"
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2028
---

À l'approche de Noël et du déballage de cadeaux, faisons un tour sur le déballage des paramètres variadiques.

{{%info%}}Ce qui suit n'est plus d'actualité depuis C++17 et les [fold expressions](https://en.cppreference.com/w/cpp/language/fold).{{%/info%}}

## Fonction récursive

La méthode habituelle pour utiliser chaque paramètre est la récursion jusqu'à plus d'argument ou jusqu'à un nombre défini, généralement 1.

Quelque chose dans ce goût-là:

```cpp
#include <iostream>

void f1()
{}

template<class T, class... Args>
void f1(const T& first, const Args&... others)
{
  std::cout << first << '\n';
  f1(others...);
}

template<class T>
void f2(const T& first)
{
  std::cout << first << '\n';
}

template<class T, class... Args>
void f2(const T& first, const Args&... others)
{
  std::cout << first << ", ";
  f2(others...);
}

int main()
{
  f1(1, 2.4, "plop");
  f2(1, 2.4, "plop");
}
```

Ce qui affiche:

```
1
2.4
plop
1, 2.4, plop
```

## Sans récursivité

Il existe cependant une autre façon de faire qui n'utilise pas la récursivité. J'ai découvert cette méthode sur le forum de openclassroms ([ce sujet, 10ème message](http://fr.openclassrooms.com/forum/sujet/atelier-quiz-question-pour-un-champion-c?page=4)), elle est très astucieuse.

Implémentée sous la forme d'une macro cela donne:

```cpp
#define UNPACK(...)                   \
  (void)::std::initializer_list<int>{ \
    (void((__VA_ARGS__)), 0)...       \
  }
```

- Le premier {{<hi cpp "void"/>}} permet d'ignorer l'`initializer_list` créé.
- {{<hi cpp "(void((__VA_ARGS__)), 0)..."/>}} évalue l'ensemble de l'expression, ignore le résulat (grâce au {{<hi cpp "void"/>}}) et retourne un caractère dans l'`initalizer_list`.
- Au final, l'expression est dépaquetée et l'`initializer_list` est rempli de {{<hi cpp "0"/>}}.

Avec cette macro les 2 fonctions précédentes deviennent:

```cpp
template<class... Args>
void f1(const Args&... args)
{
  UNPACK(std::cout << args << '\n');
}

template<class T, class... Args>
void f2(const T& first, const Args&... others)
{
  std::cout << first;
  UNPACK(std::cout << ", " << others);
  std::cout << '\n';
}
```

Ce qui simplifie considérablement l'écriture :).

Évidemment, cette macro se trouve depuis dans falcon: [falcon.cxx](https://github.com/jonathanpoelen/falcon.cxx).


## Fold expression et C++17

Depuis C++17, le langage introduit les [fold expressions](https://en.cppreference.com/w/cpp/language/fold) qui ne font pas moins que la macro précédente. Elles font même un peu plus, mais je vous laisse consulter la doc pour en prendre connaissance.

```cpp
template<class... Args>
void f1(const Args&... args)
{
  ((std::cout << args << '\n'), ...);
}

template<class T, class... Args>
void f2(const T& first, const Args&... others)
{
  std::cout << first;
  ((std::cout << ", " << others), ...);
  std::cout << '\n';
}
```

La structure du code est très proche du précédent, il n'y a que la macro qui est remplacée par une paire de parenthèse et `, ...`.

Par contre, il ne faut pas oublier que pour certain type, l'opérateur `,` peut être surchargé et engendrer des comportements indésirables. La solution consiste -- comme précédemment -- à entourer l'expression de `void`. Comme cela devient un peu moche, on recycle la macro `UNPACK`:

```cpp
#define UNPACK(...) (void((__VA_ARGS__)), ...)
```
