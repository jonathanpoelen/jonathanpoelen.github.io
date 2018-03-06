---
title: "Tableau dans un std::vector"
#description: ""
date: 2013-09-18T20:09:51+01:00
#lastmod: 2018-02-13T03:10:51+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

Prenons le type suivant: {{<hi cpp "std::vector<int[4]>"/>}}. Qui peut se justifier. À priori, cela ne cause aucun problème ; et c'est vrai !

Ajoutons maintenant un élément à notre vector avec `push_back`.

```cpp
std::vector<int[2]> v;
int a[2]{};
v.push_back(a);
```

Patatras, rien ne va plus, il y a 2 erreurs. La première concerne la construction du tableau et la seconde sa destruction car un tableau n'a ni constructeur ni destructeur.

La manière la plus facile pour éliminer ces erreurs de compilation est de mettre un wrapper sur le tableau. Écrire ce wrapper n'est pas très compliqué et Oh joie, Oh bonheur, il existe [std::array](http://en.cppreference.com/w/cpp/container/array).

Mais si l'on tient vraiment à notre tableau (pour d'obscures raisons sataniques :D) il est toujours possible de modifier le comportement du vector pour qu'il comprenne les tableaux. Ceci à travers l'allocateur, le second paramètre template d'un vector ; celui jamais utilisé, toujours oublié {{<hi cpp "std::vector<T, là_ici>"/>}}.

La technique consiste à remplacer l'allocateur par une version spécialisée pour les tableaux. Le plus simple est d'hériter d'un [std::allocator](http://en.cppreference.com/w/cpp/memory/allocator) et de redéfinir les 2 méthodes problématiques: [construct](http://en.cppreference.com/w/cpp/memory/allocator/construct) et [destroy](http://en.cppreference.com/w/cpp/memory/allocator/destroy). On peut aussi utiliser une spécialisation de template.

Et ne surtout pas oublier `rebind` qui permet de recréer l'allocateur avec un type interne différent. C'est utilisé par les conteneurs :).

(Un [précédent article]({{< ref "placement-new-allocateur-et-conteneur.md">}}) permet de comprendre les mécanismes utilisés par un allocateur.)

L'implémentation de `construct` et `destroy` est vraiment bateau, il suffit d'appeler le constructeur ou le destructeur pour chaque élément du tableau.

Mais pour faire au minimum bien les choses et supporter l'allocation de tableau de tableau ({{<hi cpp "int[3][2]"/>}}) on utilise `array_allocator::construct/destroy` si les cellules sont des tableaux (appel récursif) ou `std::allocator::construct/destroy` dans le cas contraire.

À noter que la récursivité peut être éliminée en utilisant les propriétés d'alignement des tableaux ({{<hi cpp "int[3][2]"/>}} -> {{<hi cpp "int[3*2]"/>}}).

```cpp
template<typename T>
class array_allocator;

//c'est juste pour les tableaux
template<typename T, std::size_t N>
class array_allocator<T[N]>
: public  std::allocator<T[N]>
{
public:
  using value_allocator = typename std::conditional<
    std::is_array<T>::value,
    array_allocator<T>,
    std::allocator<T>
  >::type;

public:
  template<typename U>
  struct rebind
  { typedef array_allocator<U> other; };

  void construct(T(*arr_ptr)[N], const T(&val)[N])
  {
    value_allocator alloc;
    for (std::size_t n = 0; n < N; ++n) {
      alloc.construct(&(*arr_ptr)[n], val[n]);
    }
  }

  void destroy(T(*arr_ptr)[N])
  {
    value_allocator alloc;
    for (std::size_t n = 0; n < N; ++n) {
      alloc.destroy(&(*arr_ptr)[n]);
    }
  }
};
```

Dans l'idéal, il faudrait un `construct` avec un nombre variable d'arguments ce qui permet d'utiliser `emplace_back`.
Toutefois, cette version est fonctionnelle avec `push_back` :).

```cpp
std::vector<int[3], array_allocator<int[3]> > v;
int a[3]{1,2,3};
v.push_back(a);
v.push_back({4,5,6});
```

(La bibliothèque [falcon](https://github.com/jonathanpoelen/falcon) (dont je suis le seul développeur actuellement :p) dispose d'un allocateur de ce style: [generic_allocator](https://github.com/jonathanpoelen/falcon/blob/master/falcon/memory/generic_allocator.hpp). Comme son nom l'indique, l'allocateur couvre un spectre un peu plus large et gère en plus des types POD.)
