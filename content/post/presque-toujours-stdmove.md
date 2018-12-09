---
title: "Presque toujours std::move"
#thumbnail: ""
#description: ""
date: 2018-12-09T13:11:29+01:00
#lastmod: 2018-12-07T23:11:29+01:00
slug: "presque-toujours-stdmove"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2028
---

Le principe de `std::move` est de "déplacer[^1]" un objet qui n'est plus utilisé dans l'objective de décharger la responsabilité dans une autre variable ou d'utiliser le constructeur de déplacement à la place de celui de copie.

[^1]: std::move n'est qu'un `static_cast<T&&>`

Pour avoir de meilleures performances, il est tentant de le mettre partout lorsque la variable n'est plus utilisée. Un constructeur de déplacement sera toujours préférable au constructeur de copie, pourquoi s'en priver ?

*Rire sarcastique.*

Parce qu'il est possible de "déplacer" les valeurs sans passer le constructeur !

## Du "déplacement" sans std::move

C'est fort du roquefort ! Pas de constructeur, pas de `std::move`, mais un objet qui se déplace quand même. On croirait faire de la magie !

`std::move` va forcer l'appel à un constructeur de déplacement alors que le compilateur a les moyens -- et même l'obligation -- de le faire pour nous. Ou mieux, bypasser tous les constructeurs et mettre la valeur directement là où il faut. On parle d'[élision par copie](https://en.cppreference.com/w/cpp/language/copy_elision) ou de (N)RVO (Named Return Value Optimization). Sauf que `std::move` bloque ces optimisations.

Dans le cas de l'élision de copie, le compilateur construit directement les paramètres de type T (un type sans référence) lorsqu'on utilise un temporaire.

Avec `void foo(T)`, il faut faire `foo(T{...})` et non pas `foo(std::move(T({...})`. Comme on ne fait pas `T x = std::move(T{...})`, mais `T x = T{...}` (on peut faire `T x{}`, c'est pour l'exemple)

Les temporaires peuvent aussi exister depuis l'appel à une fonction qui retourne `T`.

Avec `T bar()`, il faut faire `foo(bar())`, et non pas `foo(std::move(bar())`.

La (N)RVO est une optimisation qui fonctionne sur le même principe au niveau de la valeur de retour.

On n'écrit pas `T bar() { return std::move(T{...}); }`, mais `T bar() { return T{...}; }`.

Le compilateur possède aussi un comportement spécial pour la variable locale retournée: celle-ci doit être déplacée si possible. Et si possible, on optimise avec une élision.

On écrit

```cpp
T bar() {
  T x;
  // ...
  return x;
}
```

À la place de

```cpp
T bar() {
  T x;
  // ...
  return std::move(x);
}
```

Au final, les seuls moments où `std::move` devrait être utilisé, sont

- Pour transférer un type référence (lvalue ou rvalue) qui force la variable en une expression rvalue.
- Pour transmettre un type plein (sans référence) à une autre fonction.
