---
title: "Placement new, allocateur et conteneur"
#description: ""
date: 2013-04-22T22:17:22+01:00
#lastmod: 2018-02-13T03:19:22+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2038
---

{{<hi cpp "new"/>}} est généralement utilisé pour allouer un bloc mémoire et -- où il diffère de `malloc()`, -- appelle le constructeur de la classe demandée (si constructeur il y a). Il fait donc deux choses en une.

Mais {{<hi cpp "new"/>}} fait une troisième chose: il lance une exception `std::bad_alloc` si l'espace mémoire est insuffisant.

Ou pas. Car {{<hi cpp "new"/>}} est un opérateur surchargeable qui prend des paramètres. Le standard définit dans l'en-tête `<new>` un type (`std::nothrow_t`) et une variable (`std::nothrow`) qui permettent de retourner un pointeur nul plutôt que lancer une exception.

```cpp
Machin* machin = new (std::nothrow) Machin(/*params...*/);
```

Voici ce qui clôt l'utilisation courante et voyons comment faire chaque étape séparement.


## Allouer de la mémoire

Comme on veut juste réserver un espace mémoire, `malloc()` peut suffire, mais prenons les bonnes habitudes, utilisons la fonction [::operator new()](http://en.cppreference.com/w/cpp/memory/new/operator_new) !

```cpp
void* p = ::operator new(sizeof(Machin));
```

Ou la version sans exception.

```cpp
void* p = ::operator new(sizeof(Machin), std::nothrow);
```

Maintenant qu'on a un joli espace mémoire tout fraîchement alloué, construisons l'objet.

## Placement new

Le placement new permet d'appeler le constructeur d'un objet sur une zone mémoire prédéfinie.

```cpp
Machin* machin = new (p) Machin(/params…/);
// machin == p (même zone mémoire)
```

Mais attention aux fuites mémoire si le constructeur de `Machin` lance une exception.

Et maintenant que c'est construit, on détruit :D

## Destruction

Un appel explicite au destructeur et le tour est joué.

```cpp
machin->~Machin();
```

Étape inutile pour les types scalaires. De toute façon ils n'ont pas de destructeur. En C++17, la stl contient [std::destroy_at](http://en.cppreference.com/w/cpp/memory/destroy_at).

Évidemment, si `machin` possède un destructeur virtuel et est en réalité une classe fille, alors c'est le destructeur de la classe dérivée qui est appelé.

Il ne reste plus qu'à libérer la mémoire.

## Libération de la mémoire

De la même manière qu'il faut utiliser {{<hi cpp "delete"/>}}/{{<hi cpp "delete[]"/>}} pour {{<hi cpp "new"/>}}/{{<hi cpp "new[]"/>}}, il existe un [::operator delete()](http://en.cppreference.com/w/cpp/memory/new/operator_delete) associé à {{<hi cpp "::operator new()"/>}}.

```cpp
::operator delete (p);
```

## Allocation de tableau

{{<hi cpp "::operator new[]"/>}} et {{<hi cpp "::operator delete[]"/>}} fonctionnent de la même manière, mais sont plus sûrs qu'une gestion manuelle de tableau avec leurs homologues sans crochets. Ne serait-ce que pour détruire proprement les objets si un des constructeurs jette une exception.

```cpp
void* p = ::operator new[](sizeof(Machin) * n); // identique que la version sans crochet
Machin* machin = new(p) Machin[n]/*{params...}*/;
```

## Surcharge de new et delete

Toutes les formes de {{<hi cpp "new"/>}} et {{<hi cpp "delete"/>}} sont surchargeables de façon locale ou globale. Local quand l'opérateur est implémenté à l'intérieur d'une classe (son prototype sera implicitement statique) et global lorsqu'implémenté dans le namespace global.

De plus, comme {{<hi cpp "new"/>}} peut prendre des paramètres, il est possible de les personnaliser et d'en ajouter.

```cpp
#include <new>
struct A
{
  A(int i=0)
  {
    std::cout << "A(" << i << ")" << std::endl;
  }

  void* operator new (std::size_t size, int x, int y) throw(std::bad_alloc)
  {
    std::cout << "new A " << x << ' ' << y << std::endl;
    return ::operator new (sizeof(A));
  }
};

new(1,2) A; // affiche "new A 1 2 A(0)"
```

## L'alignement mémoire

L'alignement mémoire est une histoire à part entière, je n'en parle donc pas plus que ça :p.

Toutefois, renseignez-vous dessus, des variables non alignées peuvent faire chuter les performances et planter certaines architectures de processeur.
Présent dans boost et la dernière norme du C++, il existe [aligned_storage](http://en.cppreference.com/w/cpp/types/aligned_storage) et co pour aider.

## Allocateurs de la SL

Les allocateurs sont des objets qui s'occupent de faire tout ce qui a été dit auparavant à travers des fonctions comme `allocate`/`desallocate`, `construct`/`destroy`, etc, mais sans faire de surcharge. En fait, tous les conteneurs dynamiques de la stl utilisent un [std::allocator](http://en.cppreference.com/w/cpp/memory/allocator) comme allocateur par défaut paramétrable à travers le dernier type template du conteneur.

Depuis C++11, il existe [std::allocator_traits](http://en.cppreference.com/w/cpp/memory/allocator_traits) qui simplifie grandement la création d'un allocateur en rendant la plupart des fonctions optionnelles.
Et à partir de C++17 un [allocateur polymorphique](http://en.cppreference.com/w/cpp/header/memory_resource) voit le jour avec quelques gestionnaires de mémoire.

## Allocateurs et conteneurs

Utiliser un allocateur personnalisé permet d'avoir un contrôle plus fin de la mémoire pour répondre plus efficacement au besoin et augmenter les performances.

Évidemment, l'allocateur peut être personnalisable et dans certaines circonstances permet un gain de performance en évitant l'allocation/dés-allocation répété.

Par exemple, il y a quelques semaines, j'ai fait un algorithme qui faisait au total 2'100'000 `new` pour au final ne garder que 100'000 objets. Donc 2'000'000 de `delete`.

Dans le pire des cas, il y avait une suite de 25 objets à supprimer. Avec un allocateur qui ne vide pas la mémoire mais garde un tableau des pointeurs alloués je n'avais plus qu'à faire 25 dés-allocations au lieu de 2'000'000. Le nombre de `new` effectuées descendait quant à lui à 100'025.

Seul le nombre d'appels au destructeur et au placement new restait inchangé. Respectivement 2'000'000 et 2'100'000.

Au final l'algorithme était quand même 30% plus rapide. J'aurais aussi pu allouer directement tous mes objets et les réutiliser, mais là n'est pas le propos :).

Ce type d'optimisation reste toutefois exceptionnel et n'est pas adapté à tous les conteneurs. Par exemple, std::vector se prête mal à ce genre d'exercice car il demande toujours une allocation d'au moins la taille du nombre d'éléments qu'il possède.

Par contre, les conteneurs comme std::list ou std::map, qui allouent toujours un seul élément à la fois sont un meilleur choix pour utiliser ce type d'allocateur.
Cependant, comme les conteneurs retournent une copie de leur allocateur, il sera difficile de supprimer de manière simple la mémoire non utilisée par l'allocateur du conteneur.

En ce moment, j'ajoute plusieurs allocateurs dans [falcon/memory](https://github.com/jonathanpoelen/falcon/tree/master/falcon/memory). Même si celui dont je viens de parler n'est pas encore présent car son implémentation était vraiment basique et spécifique, il fait quand même partie de ma todo-list.
