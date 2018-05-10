---
title: "Au cœur d'un variant"
#description: ""
date: 2018-04-08T00:45:55+02:00
#lastmod: 2017-08-08T00:45:55+02:00
#tags: [ ]
categories: [ "c++" ]
#project_url: ""
#slug: "a-site-page"
#toc: false
draft: true
ghcommentid: 0
---

Les prochains articles vont être consacrés à la réalisation d'un classe variant comme on peut la trouver dans la [stl](http://en.cppreference.com/w/cpp/utility/variant), [boost](http://en.cppreference.com/w/cpp/utility/variant) et [autres](https://github.com/mapbox/variant). Il existe de nombreuses techniques plus ou moins simple à réaliser et plus ou moins coûteuse à l'exécution. Je vais éssayer de faire le tour de ce que j'ai pu voir et, par la même occasion, faire quelques expérimentations.

## Rappel sur ce qu'est un variant

Un variant est une **union sécurisée** comme on peut le trouver dans les langages fonctionnels.
Contrairement au [union](TODO) classique du C++, le variant garde l'information du type manipulé.
C'est en cela qu'il est sécurisé, on ne sélectionne pas une valeur d'un certain type,
mais on fournit des fonctions de traitement correspond à chaque types pour laisser le variant sélectionner la fonction lié au type enregistré.

Même si le variant peut aisément remplacer l'héritage lorsque le nombre de classe dérivée est connue,
il est beaucoup plus adapté lorsque les valeurs priment sur les comportements.
Par exemple, une valeur d'une structure JSON représente un nombre, un tableau, une chaîne de caractères ou un objet.
Il n'y a pas de comportement commun, les traitements se feront en fonction du type de la valeur.


## Un premier variant

TODO info
Pour ne pas faire fuir d'éventuel lecteur, la version présentée ici est la pire qui soit.
Elle servira néanmois de base de travail et sera paufinée tout au long des articles pour atteindre l'*idéal* du variant.

Notre version minimal de variant va contenir:

- Les constructeurs par défaut, de copie et de déplacement.
- Un constructeur pour initialiser avec un type du variant.
- Les opérateurs d'affectation correspondant aux constructeurs.
- Une fonction `visit`. En membre pour des raisons de simplification.

Comme le type change en cours de route, nous allons utiliser une classe de base qui pour chaque dérivée va contenir le type réel.
Grâce à cela, le type stocker pourra être supprimé et un nouveau type pourra y être enregistré.
Cette classe de base pourra aussi servire à implémenter l'opérateur de copie via une fonction `clone`.

{{<fhi "cpp_variant/variant1.cpp" variant1>}}

Pour ne pas parasité les codes, je n'ajoute pas les noexcept potentiellement possible. De toute façon, avec les allocations dynamique, cela va être diffcile.

Toute la difficulté va se trouver dans les implémentations de `VariantBase` et la fonction `visit`. Pour en faire une dérivée, un pattern assez commun va être utilisé. Celui d'avoir une classe `VariantImpl` template sur le type à stocker.

{{<fhi "cpp_variant/variant1.cpp" variant1_impl>}}

En réalité ce qu'on vient de faire ici n'est ni plus n'y moins qu'un `std::any`. Si on réfléchie bien, nous ne somme pas limité dans les types à stocker et il n'y a aucune vérification au niveau du variant. C'est mal, mais on va rester comme cela pour le moment.

Reste ensuite la fonction `visit`. À ce stade, je dirais, que la solution la plus naturelle est d'utiliser `dynamic_cast` pour déterminer le type réel et appeler la bonne surcharge de fonction.

{{<fhi "cpp_variant/variant1.cpp" variant1_visit1>}}

J'avoue avoir fait de grosse concession ici, notament le type de retour qui est toujours `void` au lieu du retour de `f`. Normalement il faut utiliser une solution récursive qui aurait aussi l'avantage d'arrêter la condition au premier test réussi.

{{<fhi "cpp_variant/variant1.cpp" variant1_visit2>}}

Cette implémentation retourne bien la valeur de `f`. Il y a même une petit optimisation avec la condition: dynamic_cast n'est pas utilisé sur le dernier élément.
Comme notre variant ne contient -- normalement -- qu'un nombre restreint de type, si la valeur de `impl_` n'est pas du type de la liste moins un élément, alors elle est forcément du type du dernier élément.







## Choix de la zone de stockage

Il y a 2 manières de stocker une donnée, le tas et la pile. Stocker sur le tas se fait avec une allocation dynamique alors que pour la pile, il faut calculer la taille maximal occupée par un type pour réserver un emplacement à l'intérieur du variant (comme le fait une union).

L'allocation dynamique est très simple à mettre en place, mais comme nous le verrons, ce mécanisme n'est pas adapté au variant. Cette version permettra néanmois d'avoir une base avec lequelle travailler.


```cpp
template<class... Ts>
class Variant
{
}
```



Alors que c'est une solution adaptée à [std::any](http://en.cppreference.com/w/cpp/utility/any).
une bonne s'avère être la pire solution. En réalité, c'est un mécanis
pose quelques problèmes



La première question à se poser est comment stocker une valeur ?



Question anodine mais inévitablement relier à une autre question: "Comment garder le type réelement stocké ?"



La première

- char[] / std::union_storage...
- union recursive
  - linéaire
  - arbre
- base et héritage: perte du type
- petit mot sur les variants récursifs

marquer le type

- typeid et dynamic_cast
- pointeur de fonction (+ utilisation pour dtor)
- indice (+ pb avec les subvariants)

dispatcheur

- virtual
  - standard
  - tableau de pointeur de fonction
- condition récursive (pour union: en fonction du choix initial)
- switch (posibilité de faire sans condition /!\ code machine plus long)
  - avec peu et beaucoup de type
  - type le plus petit possible
    - indiquer le nombre de bit utilisés a-t-il un impact ?
    - et avec un masque explicite ?

affectation

- self-assignment et réutilisation du type (dtor/ctor vs op= quand possible)

destruction

- pas toujours obligatoire si destructeur trivial
  - ne pas les prendre en compte dans les conditions a-t-il un impact ?

différence entre demander le type pour appliquer une fonction et demander d'appliquer une fonction selon le type

- résistance au changement (=erreur)
- performance
