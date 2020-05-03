---
title: "Au cœur d'un variant"
slug: "au-coeur-dun-variant"
#description: ""
date: 2018-09-02T14:45:55+02:00
#lastmod: 2017-08-08T00:45:55+02:00
#tags: [ ]
categories: [ "c++" ]
#project_url: ""
#slug: "a-site-page"
#toc: false
draft: false
ghcommentid: 0
expire: 2028
---

Cet article va être consacré à la réalisation d'une classe variant comme on peut la trouver dans la [STL](http://en.cppreference.com/w/cpp/utility/variant), [boost](http://en.cppreference.com/w/cpp/utility/variant) et [autres](https://github.com/mapbox/variant). Il existe de nombreuses techniques plus ou moins simples à réaliser et plus ou moins coûteuses à l'exécution. Je vais faire un petit tour de ce que j'ai pu voir et comment les implémenter.


## Rappel sur ce qu'est un variant

Un variant est une **union sécurisée** comme on peut le trouver dans les langages fonctionnels.
Contrairement aux [union](http://en.cppreference.com/w/cpp/language/union) classique du C++, le variant garde l'information du type manipulé.
C'est en cela qu'il est sécurisé, on ne sélectionne pas une valeur d'un certain type,
mais on fournit une fonction que le variant appelle avec le type enregistré.

Même si le variant peut aisément remplacer l'héritage lorsque le nombre de classe dérivée est connue,
il est beaucoup plus adapté lorsque les valeurs priment sur les comportements.
Par exemple, une valeur d'une structure JSON représente un nombre, un tableau, une chaîne de caractères ou un objet.
Il n'y a pas de comportement commun, les traitements se feront en fonction du type de la valeur.


## Un premier variant

Notre version minimale de variant va contenir:

- Les constructeurs par défaut, de copie et de déplacement.
- Un constructeur pour initialiser avec un type de la liste.
- Les opérateurs d'affectation correspondant aux constructeurs.
- Une fonction `visit` (en membre, pour des raisons de simplification).

{{%info%}}
  L'implémentation qui va suivre se veut simple et surtout naïve. De ce fait, elle est totalement inefficace et montre l'exemple à ne pas suivre.
  Elle servira néanmoins de base de travail et sera peaufinée tout au long des chapitres pour atteindre l'*idéal* du variant.
{{%/info%}}

Comme le type change en cours de route, nous allons utiliser en interne une classe de base qui pour chaque dérivée va contenir le type réel.
Grâce à cela, le type stocké pourra être supprimé et un nouveau type pourra y être enregistré.
Cette classe de base pourra aussi servir à implémenter l'opérateur de copie via une fonction `clone`.

{{<fhi "cpp_variant/variant1.cpp" prototype>}}

Pour ne pas parasiter les codes, je n'ajoute pas les noexcept. De toute façon, avec les allocations dynamiques, cela ne va pas être évident.

Toute la difficulté va se trouver dans les implémentations de `VariantBase` et la fonction `visit`. Pour en faire une dérivée, un pattern assez commun va être utilisé, celui d'avoir une classe `VariantImpl` template sur le type à stocker.

{{<fhi "cpp_variant/variant1.cpp" impl>}}

En réalité ce qu'on vient de faire ici n'est ni plus ni moins qu'un `std::any`. Si on réfléchit bien, nous ne sommes pas limités dans les types à stocker et il n'y a aucune vérification au niveau de l'initialisation d'une valeur. C'est mal, mais on va rester comme cela pour le moment.

Reste ensuite la fonction `visit`. À ce stade, je dirais que la solution la plus naturelle est d'utiliser `dynamic_cast` pour déterminer le type réel et appeler la bonne surcharge de fonction.

{{<fhi "cpp_variant/variant1.cpp" visit>}}

Cette implémentation parcourt récursivement les types du variant pour trouver celui qui correspond à la valeur de `impl_`, appel `f` avec le bon type puis propage son retour en remontant la pile d'appel.
Le dernier élément est un cas spécial traité dans le `else` car, quand on le compare avec `dynamic_cast`, le résultat est toujours vrai.
Comme notre variant ne contient --normalement-- qu'un nombre restreint de types, si la valeur de `impl_` ne correspond pas aux types qui précèdent le dernier, alors `impl_` est forcément du type du dernier élément.


## Moi j'aime pas dynamic_cast

`dynamic_cast` est souvent un signe révélateur d'un problème de conception. Si on abstrait les valeurs, c'est dans le but de ne pas se soucier du type de l'implémentation. Or, un variant met le focus sur le type et rend caduque cette abstraction. Seulement, `dynamic_cast` a un coût d'exécution exorbitant par rapport à la tâche qu'il effectue ici.

De ce fait, `dynamic_cast` n'est pas une bonne solution, il est plus judicieux de conserver une information pour différencier les types. Comme un variant contient une liste d'éléments, l'indice du type utilisé suffit amplement.

```cpp
class Variant
{
  // ...
private:
  std::unique_ptr<detail::VariantBase> impl_;
  std::size_t type_index_;
}
```

Maintenant, il faut convertir un type en indice, c'est à ce moment que la méta-programmation arrive à la rescousse.

{{<fhi "cpp_variant/variant2.cpp" mp_index_of >}}

`mp_index_of` est un alias sur `std::integral_constant`. L'implémentation déroule récursivement les éléments de `Ts` jusqu'à trouver `T` et retourne le nombre d'éléments qu'il reste dans la liste. Soustraire ce résultat à `sizeof...(Ts) - 1` permet d'avoir la position de `T`.

On met à jour l'implémentation pour initialiser le nouveau membre.

{{<fhi "cpp_variant/variant2.cpp" impl >}}

Puis on supprime `dynamic_cast` de la fonction `visit`, le remplaçant par une comparaison d'index.

{{<fhi "cpp_variant/variant2.cpp" visit >}}

Peu de changement finalement, mais maintenant `variant` peut fonctionner sans support de [RTTI](https://fr.wikipedia.org/wiki/Run-time_type_information) !

On notera aussi que puisque nous possédons l'indice lié au type, on peut aussi remplacer les fonctions virtuelles par un appel à `visit` pour supprimer la vtable et enlever l'indirection pour accéder aux fonctions virtuelles dans celle-ci.


## L'allocation dynamique n'est pas gratuite

Il est vrai que l'allocation dynamique a un coût non négligeable sur les performances. Personne n'a idée de faire `new int` alors qu'en regardant de plus près, c'est exactement ce que fait notre implémentation. Vient ensuite les déréférencements de pointeur qui font sauter des optimisations. Effet amplifié lorsque les fonctions sont `virtual`. Décidément, l'allocation dynamique pour un variant n'est pas une bonne idée.

Le mieux serait de stocker nos types de la même manière qu'une union: un seul bloc mémoire de la taille du type le plus grand. À ma connaissance il existe 2 possibilités:

- une union récursive
- [std::aligned_union](http://en.cppreference.com/w/cpp/types/aligned_union)

Pour choisir le procédé le plus efficace, nous implémentons les 2 dans une classe qui ne possède que les fonctions d'accès et les constructeurs.

{{<fhi "cpp_variant/union1.cpp" aligned_union >}}

Sans les constructeurs, la version avec `std::aligned_union` est vraiment simple. Mais l'utilisation de `reinterpret_cast` empêche de mettre la fonction `get()` en `constexpr` (gcc l'accepte néanmoins).

À contrario, la version avec une union récursive est extrêmement verbeuse (toujours sans constructeur d'initialisation de valeur):

{{<fhi "cpp_variant/union1.cpp" union >}}

Si on regarde [l'assembleur](https://godbolt.org/g/xu9fwh), il s'avère que les 2 versions sont exactement les mêmes.

Pour initialiser l'objet avec une valeur, la version avec `std::aligned_union` doit utiliser un [placement new](http://en.cppreference.com/w/cpp/language/new) qui empêche de rendre le constructeur `constexpr`. Ce qui par la même occasion s'applique aussi au variant. Sans compter le problème du `reinterpret_cast` dans la fonction `get()`. Du coup, bien que cette version soit plus simple et que je ne mette pas `constexpr`, l'union récursive est préférable.

{{<fhi "cpp_variant/variant3.cpp" in_place_index_t >}}

(`RecursiveUnion` devient `VariadicUnion`)

{{<fhi "cpp_variant/variant3.cpp" ctor_VariadicUnion >}}

Puis on adapte les fonctions de `Variant`.
Le code final est plutôt gros alors je ne mets {{<urlhi "cpp_variant/variant3.cpp#L157" "que le lien">}}.

Pour éviter une condition particulière dans le code, l'union possède un membre supplémentaire: `Uninit`, utilisé par `init`, `copy` et `destroy` pour représenter un variant sans valeur.

Il y a aussi une condition dans `operator=` pour choisir entre la fonction `copy` si les 2 éléments sont du même type ou les fonctions `destroy`+`init` dans le cas contraire. Cette condition peut être supprimée si:

- Tous les éléments ont un destructeur trivial: il n'y a pas besoin de faire `destroy`+`init`.
- La fonction `visit` peut prendre plusieurs variants en paramètre pour faire un switch allant de 0 à `(sizeof...(Ts) + 1) * (sizeof...(Ts) + 1)`.


## Mot de la fin

Bien que le variant actuel soit incomplet, il est utilisable et proche des implémentations actuelles. Mais il y a plusieurs petits détails qui ne sont pas approfondis ici:

- l'optimisation sur la taille de `type_index`,
- les différents moyens de remplacer une vtable (ici je n'utilise que le if/else récursif),
- le coût d'utilisation d'un objet en fonction de sa nature (par exemple: le compilateur dévirtualise-t-il les fonctions virtuelles venant d'un membre de variant ?)
- les variants récursives
- et bien d'autres

Les prochains articles seront davantage axés sur la méta-programmation et indirectement reliés avec certains aspects du variant présentés ici.

Les sources sont disponibles sur {{<urlhi "cpp_variant" "github">}}.
