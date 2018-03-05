---
title: "Appel conditionnel de fonction selon la validité d'une expression"
#description: ""
date: 2014-11-30T19:17:52+01:00
#lastmod: 2018-02-13T03:38:52+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

L'approche suivante consiste à vérifier qu'une fonction (membre ou statique) est appelable dans le but de l'utiliser, ou, à défaut, fournir implémentation générique. De manière plus générale, la méthode présentée ici s'applique à toutes expressions.

## Appeler T::sort si possible, sinon std::sort(begin(T), end(T))

L'exemple va se faire sur la classe `std::list` qui n'est pas triable avec `std::sort`, mais possède une fonction membre `sort()`. Ainsi que sur `std::vector` qui, inversement, n'a pas de fonction membre `sort()`, mais fonctionne avec `std::sort`.

La méthode est simple et consiste à créer 2 fonctions: une pour vérifie si une expression est valide (ici `x.sort()`) et une autre en cas d'échec.

Seulement, qui dit 2 fonctions dit 2 prototypes. Leur prototype doit être légèrement différent mais compatible avec les mêmes valeurs d'entrée pour appeler la seconde si la première échoue (principe du [SFINAE](http://en.cppreference.com/w/cpp/language/sfinae)).

Pour vérifier l'expression, seul 2 mots clef existent: {{<hi cpp "sizeof"/>}} et {{<hi cpp "decltype"/>}}. Cette procédure est donc possible avant C++11, même si {{<hi cpp "sizeof"/>}} requière un peu d'enrobage.

```cpp
#include <algorithm>

//avec decltype
template<class Container>
auto dispatch_sort(Container& c, int)
-> decltype(void(c.sort())) // force decltype au type void
{ c.sort(); }

template<class Container>
void dispatch_sort(Container& c, unsigned)
{
  using std::begin;
  using std::end;
  std::sort(begin(c), end(c));
}

template<class Cont>
void sort(Cont& c)
{ dispatch_sort(c, 1); }
```

La fonction `sort` appel `dispatch_sort` avec un {{<hi cpp "int"/>}} (la valeur n'importe pas, seul le type compte). Comme la seule différence des 2 fonctions `dispatch_sort` est le premier paramètre, le prototype avec un {{<hi cpp "int"/>}} correspond parfaitement.

Si une fonction membre `sort` existe, alors l'expression dans {{<hi cpp "decltype"/>}} est valide et la fonction appelé. Dans le cas contraire, le compilateur cherche une fonction avec des paramètres pouvant être conpatible. Le {{<hi cpp "int"/>}} pouvant être convertit en {{<hi cpp "unsigned"/>}}, le compilateur se rabat sur le second prototype qui fait appel à `std::sort`.

Le point clef étant de mettre toutes les informations dans le prototype. J'aurais par exemple put mettre {{<hi cpp "decltype"/>}} dans un paramètre initialisé avec une valeur par défaut ({{<hi cpp "f(int, decltype(xxx)* = 0);"/>}}, mais il faudra probablement ajouter {{<hi cpp "std::remove_reference"/>}} car un pointeur sur une référence n'est pas permis).

## Programme de test

```cpp
#include <iostream>
#include <vector>
#include <list>

int main()
{
  std::vector<int> v({2,6,4});
  std::list<int>   l({2,6,4});

  sort(v);
  sort(l);

  for (auto i : v) std::cout << i << ' ';
  std::cout << '\n';
  for (auto i : l) std::cout << i << ' ';
}
```

Résultats:

```
2 4 6
2 4 6
```

## implémentation avec sizeof (pre-C++11)

J'ai indiqué qu'il été possible d'utiliser {{<hi cpp "sizeof"/>}} à la place de {{<hi cpp "decltype"/>}}. Voici comment:

```cpp
template<std::size_t, class T = void>
struct dispatch_result_type
{ typedef T type; };

template<class T>
T declval();

template<class Container>
typename dispatch_result_type<
  sizeof(void(declval<Container&>().sort()),1)
>::type
dispatch_sort(Container& c, int)
{ c.sort(); }
```

Le {{<hi cpp ",1"/>}} de {{<hi cpp "sizeof(xxx,1)"/>}} peut dérouter mais est requis si l'expression `xxx` retourne {{<hi cpp "void"/>}}. Comme {{<hi cpp "void"/>}} n'est pas vraiment un type, il ne fonctionne pas avec {{<hi cpp "sizeof"/>}} et il faut donc lui fournir autre chose. Il faut bien comprendre qu'ici {{<hi cpp "xxx,1"/>}} est **une seule** expression et non pas 2 paramètres.

Bien que très peu probable, si j'ai mit {{<hi cpp "void(yyy)"/>}}, c'est pour prévenir la surcharge de l'opérator '`,`' sur le type de retour retourné par `yyy` (car cet opérateur peut lui-même retourner un {{<hi cpp "void"/>}}).

{{<hi cpp "sizeof"/>}} ne donne pas l'information sur le type de retour mais une valeur, il est couplé à {{<hi cpp "dispatch_result_type"/>}} qui prend en second paramètre template le type de retour ({{<hi cpp "void"/>}} par défaut). Quand à {{<hi cpp "declval"/>}}, c'est le même principe que [celui de la SL](http://en.cppreference.com/w/cpp/utility/declval).
