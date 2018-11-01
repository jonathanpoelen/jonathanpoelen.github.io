---
title: "La récursivité et le mauvais exemple de Fibonacci"
slug: "la-recursivite-et-le-mauvais-exemple-de-fibonacci"
#description: ""
date: 2012-08-16T02:22:24+01:00
#lastmod: 2018-02-13T02:14:17+01:00
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2058
---

Quasiment toute personne ayant suivi un cours sur la récursivité a eu un exercice de la forme:

> Coder la [suite de Fibonacci](https://fr.wikipedia.org/wiki/Suite_de_Fibonacci) en récursive et en itérative.

Mais partout où j'ai vu une implémentation récursive, je suis tombé sur un algorithme inefficace. Voici ce qu'on peut trouver.

```c
//itérative
long long fib(unsigned n)
{
  if (n == 0)
    return n;
  long long a = 0, b = 1, tmp;
  while (--n)
  {
    tmp = a + b;
    a = b;
    b = tmp;
  }
  return b;
}

// récursive
long long fib_r(unsigned n)
{
  if (0 == n || 1 == n)
    return n;
  return fib_r(n-1) + fib_r(n-2);
}
```

Sauf que cet algorithme récursif est pourri. Il recalcule sans cesse `fib-1` et `fib-2` et fait monter la pile d'appel **jusqu'à explosion** là où l'algorithme itératif additionne une variable.

Pour avoir un algorithme récursif équivalent à celui itératif, il faut garder le même comportement. C'est à dire garder `a` et `b`.

```c
long long fib_r_impl(unsigned n, long long r, long long rp)
{
  if (0 == n)
    return r;
  return fib_r_impl(n-1, r+rp, r);
}

long long fib_r(unsigned n)
{
  return fib_r_impl(n, 0, 1);
}
```

Cet algo est strictement identique au premier car applique la [récursion terminale](http://fr.wikipedia.org/wiki/R%C3%A9cursion_terminale). Ainsi, le code généré par un compilateur (avec les options d'optimisation) donnera un binaire identique (ou très proche). Des langages comme OCaml ou Haskel optimisent la pile quand il y a une récursion terminale.
