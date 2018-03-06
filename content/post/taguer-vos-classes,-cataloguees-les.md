---
title: "Taguer vos classes, cataloguées-les"
#description: ""
date: 2012-12-20T01:22:50+01:00
#lastmod: 2018-02-13T02:44:50+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

Le C++ a l'avantage de faire de la surcharge de fonction et permet ainsi de spécifier des algorithmes selon des critères. Ici ce seront des "tags".

Comme exemple je vais utiliser les [tags présents dans les itérateurs de la stl](http://en.cppreference.com/w/cpp/iterator) et une implémentation de la fonction [std::advance()](http://en.cppreference.com/w/cpp/iterator/advance).

## Première implémentation

La fonction `std::advance()` permet d'incrémenter un itérateur de `N` éléments (ou décrémenter si `N` est négatif). D'après cette description, un premier algorithme peut être émis:

```cpp
template<class InputIt, class Distance>
void advance(InputIt& it, Distance n)
{
  if (n < 0){
    while (n++)
      --it;
  }
  else {
    while (n--)
      ++it;
  }
}
```


## Optimisation

Maintenant, que se passe-t-il quand l'itérateur est un pointeur ? Réponse: une boucle.

Ne serait-il pas préférable de faire `it += n` !?

C'est là que les tags entrent en jeu.


## Utilisation des tags

Les itérateurs possèdent des types prédéfinis qui sont: `pointer`, `value_type`, `reference`, `difference_type` et `iterator_category`. C'est ce dernier qui correspond au tag de l'itérateur.

En réalité un tag est une classe vide. Cela suffit, le type est porteur de l'information. Les [tags des itérateurs](http://en.cppreference.com/w/cpp/iterator/iterator_tags) possèdent aussi une hiérarchie et certains sont donc hérités.

Pour récupérer le type contenu dans un itérateur il faut passer par `std::iterator_traits<>`, cela permet de fonctionner même avec un type scalaire comme le pointeur qui ne possède pas de type interne.

Le tag d'un pointeur étant `std::random_access_iterator_tag` voici une implémentation:

```cpp
template<class It>
std::iterator_traits<It>::iterator_category
iterator_category(const It&)
{
  return typename std::iterator_traits<It>::iterator_category();
}

template<class RandomIt, class Distance>
void advance(RandomIt& it, Distance n, std::random_access_iterator_tag)
{
  it += n;
}

template<class InputIt, class Distance>
void advance(InputIt& it, Distance n)
{
  advance(it, n, iterator_category(it));
}
```

Bien sûr, on change aussi le prototype de la précédente implémentation.

```cpp
template<class InputIt, class Distance, class Tag>
void advance(InputIt& it, Distance n, Tag);
```

## Problème avec ForwardIterator

Toutefois un problème persiste, lorsqu'un ForwardIterator est utilisé, il n'y a pas d'opération de décrémentation, et la compilation ne se fait pas. Il faut de nouveau changer notre première implémentation pour qu'elle ne fonctionne qu'avec `bidirectional_iterator_tag` et une implémentation généraliste qui fait une incrémentation.

```cpp
template<class InputIt, class Distance, class Tag>
void advance(InputIt& it, Distance n, Tag)
{
  while (n--)
    ++it;
}

template<class InputIt, class Distance>
void advance(InputIt& it, Distance n, std::bidirectional_iterator_tag)
{
  if (n < 0){
    while (n++)
      --it;
  }
  else {
    while (n--)
      ++it;
  }
}
```

## Conclusion

Les tags sont un moyen simple de spécifier les comportements liés à une classe. On peut les voir comme une liste d'interface disponible pour cibler précisément ce qu'il est possible de faire ou ne pas faire.
