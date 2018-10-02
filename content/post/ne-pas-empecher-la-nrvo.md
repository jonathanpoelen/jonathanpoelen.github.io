---
title: "Ne pas empêcher la NRVO"
slug: "ne-pas-empecher-la-nrvo"
#description: ""
date: 2013-03-17T17:12:57+01:00
#lastmod: 2018-02-13T03:20:57+01:00
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

La NRVO et la RVO sont des optimisations des compilateurs pour retourner un objet sans le copier. Je renvoie directement sur la [FAQ C++ developpez.com](https://cpp.developpez.com/faq/cpp/?page=Optimisation#Qu-est-ce-que-la-RVO).

Cependant, ces optimisations ne s'appliquent que sur une variable de type `T` **sans référence**. Ce qui veut dire qu'une référence ne sera pas optimisée.

```cpp
iterator operator+(const iterator& other, int n)
{
  return iterator(other) += n; // pas de RVO
}
```

Alors qu'une décomposition de la fonction active la NRVO.

```cpp
iterator operator+(const iterator& other, int n)
{
  iterator ret(other);
  ret += n;
  return ret;
}
```

C'est une optimisation facile à faire et il est tout aussi facile de passer à côté ;).
