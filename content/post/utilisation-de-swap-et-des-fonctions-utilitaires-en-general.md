---
title: "Utilisation de swap et des fonctions utilitaires en général"
slug: "utilisation-de-swap-et-des-fonctions-utilitaires-en-general"
#description: ""
date: 2013-07-20T11:18:49+01:00
#lastmod: 2018-02-13T03:11:49+01:00
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2038
---

Une fonction utilitaire est une fonction libre qui n'étant pas attachée à une classe particulière comme une fonction de conversion (to_string, etc) ou un accesseur externe à une classe (get, begin, end, etc).

Toutes ces fonctions citées sont disponibles en C++11. La seule fonction utilitaire qui me vient à l'esprit en C++03 est `std::swap` (fonction qui échange le contenu de 2 variables). Pour info, l'en-tête de `std::swap` est passé de `<algorithm>` en C++03 à `<utility>` en C++11.

## Comment déclarer et utiliser une fonction utilitaire

Il arrive qu'un jour ou l'autre on veuille faire une surcharge de `swap` pour un objet particulier. À ce moment 2 choix s'offrent: en faire une fonction libre dans le namespace où se trouve la classe ou dans le namespace `std`. Évidemment, le meilleur choix est le premier.

Et là, pour toutes les personnes qui ont en horreur le `using namespace std` ou travaillant dans les .h, un problème va se poser. Comme `swap` se trouve dans le namespace de la stl, il est logique de faire `std::swap(...)`. Mais à ce moment, la fonction `swap` spécialisée dans le namespace de la classe n'est pas utilisée...

```cpp
#include <iostream>
#if __cplusplus >= 201103L
# include <utility>
#else
# include <algorithm>
#endif

namespace my {
  struct A{};
  void swap(A&, A&)
  { std::cout << "ok\n"; }
}

int main()
{
  my::A a, b;
  std::swap(a,b);
}
```

N'affiche rien... :/.

Par contre ce qui suit affiche "ok".

```cpp
int main()
{
  my::A a, b;
  swap(a,b);
}
```

Comme une fonction `swap` prenant des `my::A` existe, elle est utilisée. Cela est possible car la fonction `swap` fait partie du namespace `my` et que les variables `a` et `b` sont des instances d'une classe du même namespace.

Alors que ceci ne compile pas.

```cpp
int main()
{
  int a, b;
  swap(a,b);
}
```

Donc, d'un côté on a un `swap` générique dans la stl et de l'autre un `swap` spécialisé dans `my`.
Et surtout, 2 syntaxes différentes.

Dans un contexte générique (typiquement des templates) et de maintient de code, 2 formes, ce n'est pas acceptable.
Il faudrait que `std::swap` soit utilisé si aucun `swap` spécialisé n'existe.

Pour ce, on "déplace" `std::swap` dans le scope courant avec {{<hi cpp "using"/>}} et grâce l'[ADL (Argument-dependent lookup)](http://en.cppreference.com/w/cpp/language/adl), le compilateur appellera la bonne fonction.

```cpp
int main()
{
  using std::swap;
  {
    my::A a, b;
    swap(a,b); //affiche ok
  }
  {
    int a, b;
    swap(a,b); //compile
  }
}
```

Et ceci s'applique pour toutes les fonctions libres.

## Simplifier l'utilisation

La nécessité de "déplacer" les fonctions dans le scope est lourd et facile à oublier.

L'idéal serait la présence d'une unique fonction en charge d'appeler la bonne surcharge. La solution consiste en la création d'un namespace dans lequel la fonction générale est exportée et où une fonction intermédiaire l'appelle. Comme la règle d'ADL s'applique, la fonction surchargée sera appelée si existante.

```cpp
#include <algorithm>

namespace fn {
  namespace adl_barrier {
    using std::swap;

    template<class T>
    void swap_impl(T& a, T& b)
    { swap(a,b); }
  }

  template<class T>
    void swap(T& x, T& y)
    { ::fn::adl_barrier::swap_impl(x,y); }
  }
}
```

Cependant, cette solution souffre de l'effet inverse: la fonction ne peut pas être déplacée dans le même scope qu'une fonction généraliste. Il y aurait 2 prototypes identiques, le compilateur ne pourrait pas lever l'ambiguïté. Comme il est très courant de voir `using namespace std;`, un `using fn::swap` dans le même scope rentrerait en conflit avec `std::swap` lors de l'appel (aucun problème si dans un sous-scope).

```cpp
#include <iostream>

namespace my {
  struct A{};
  void swap(A&, A&)
  { std::cout << "ok\n"; }
}


int main()
{
  {
    my::A a, b;
    fn::swap(a,b); // affiche ok
  }
  {
    int a, b;
    fn::swap(a,b); // compile
  }

  {
    using fn::swap;
    using std::swap;
    int a, b;
    swap(a,b); // ambiguïté
  }

  {
    using std::swap;
    {
      using fn::swap;
      my::A a, b;
      fn::swap(a,b); // affiche ok
    }
  }
}
```
