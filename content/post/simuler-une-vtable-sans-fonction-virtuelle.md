---
title: "Simuler une vtable sans fonction virtuelle"
#description: ""
date: 2018-11-1T20:42:09+02:00
#lastmod: 2018-10-19T20:42:09+02:00
slug: "simuler-une-vtable-sans-fonction-virtuelle"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2028
---

La `vtable` est le mécanisme interne utilisé par C++ pour implémenter les fonctions virtuelles. Lorsqu'une classe possède une fonction virtuelle, un pointeur sur la `vtable` (virtual table) est automatiquement réservé par le compilateur. Cette table contient des pointeurs de fonction sur l'ensemble des fonctions virtuelles de la classe et chaque classe dérivée possède sa propre `vtable`.

Pour une classe de base implémentant une fonction `foo` virtuelle comme ci-dessous, l'utilisation de {{<hi cpp "obj->foo(/*params...*/)"/>}} va être remplacé par quelque chose proche de {{<hi cpp "cast<FunctionType>(obj->__vtable[0])(obj/*, params...*/)"/>}}.

```cpp
struct Base
{
  virtual void foo() = 0;
};
```

Chaque classe dérivée qui ajoute des fonctions virtuelles réserve des cases supplémentaires dans son tableau. Le compilateur se basant sur les types utilisés pour garantir la validité des indices.

Ce fonctionnement est très économique : un pointeur pour chaque instance d'une classe et une vtable en lecture seule par classe. C'est également très facile d'ajouter de nouvelles tables, y compris dynamiquement par le chargement de bibliothèque. Mais il y a aussi des inconvénients comme une fragmentation mémoire au niveau de la position des tables et un double déréférencement pour accéder au pointeur de fonction.

Pour la suite on se base sur une interface avec une fonction draw: {{<hi cpp "void draw(std::ostream&) const"/>}} et 2 objets ce qu'il a de plus classique: `Rect` et `Circle`.

{{<fhi "simu_vtable/shapes1.hpp" shape>}}

## Une vtable à la mano

Notre vtable va contenir des pointeurs de fonction. Le type commun d'une majorité de pointeurs est `void*`. Sauf que les pointeurs de fonction sont une exception, il n'est pas légal de les transformer en `void*` -- même si en interne le compilateur peut se le permettre pour sa vtable. Par contre, on peut récupérer l'adresse du pointeur de fonction pour avoir un pointeur de pointeur de fonction. Cela fait beaucoup de pointeurs et d'indirections alors le tableau sera remplacé par un tuple, car finalement on connait exactement les types de fonction que contient la vtable.

{{<fhi "simu_vtable/e1.cpp" vtable_type>}}

Maintenant qu'on a un type pour la vtable, il faut pouvoir la construire à partir de n'importe quel type qui pourrait supporter cette interface.

{{<fhi "simu_vtable/e1.cpp" drawable_vtable_for>}}

Grâce à ses variables inline, on peut construire notre tableau très facilement. Il ne reste plus qu'à faire un petit wrapper pour utiliser `Rect` et `Circle` indistinctement.

{{<fhi "simu_vtable/e1.cpp" drawable>}}

`Drawable` pourrait contenir directement `Rect` ou `Circle` plutôt qu'avoir un `void*`, mais pour supporter n'importe quel type et être optimisé, il est préférable d'utiliser [SBO (Small Buffer Optimization)](https://akrzemi1.wordpress.com/2014/04/14/common-optimizations/) Dans une optique de simplification, la classe n'accepte que des références et garde un pointeur. On peut la considérer comme une vue polymorphique.

Une petite démonstration de la chose:

{{<fhi "simu_vtable/e1.cpp" main>}}

C'est tout :).

Il est possible de rendre plus simple la déclaration de la classe `Drawable` avec des macros pour supprimer 2/3 du code, mais je passe mon tour.

Par contre, il y a peut-être moyen d'avoir mieux pour la vtable.


## Une vtable par type vs une vtable par instance

Notre `Drawable` contient une référence sur la vtable qui est elle-même un tableau de 1 pointeur de fonction. Autant jeter la vtable et avoir directement le pointeur de fonction comme membre. C'est plus direct et le code est plus court.

{{<fhi "simu_vtable/e2.cpp" drawable>}}

Pour une classe qui ne possède qu'une fonction polymorphique, le pointeur de fonction en membre est un peu plus efficace qu'une vtable en évitant une indirection. `std::function` utilise le même principe en interne en plus de prendre l'ownership de l'objet. Prendre l'ownership implique de mettre l'objet en membre, si l'objet prend trop de place, comme une lambda avec une grosse capture, alors elle déborde du tempo interne et une allocation dynamique est généralement effectuée. Pour un comportement plus proche des exemples ci-dessus qui prennent une référence sur les données, il existe [fn2::function_view](https://github.com/Naios/function2#small-functor-optimization).

Par contre, pour chaque fonction polymorphique, la classe doit contenir un pointeur de fonction ce qui augmente sa taille. Si cet objet est mis dans un vector, alors il y aura beaucoup de pointeurs similaires entre les valeurs et une vtable peut réduire drastiquement la mémoire allouée par le vecteur.

Il y a un dernier point qui n'est pas visible avec les exemples du dessus : le comportement peut être dynamiquement modifié. C'est-à-dire qu'une instance peut décider de modifier le comportement d'une de ses fonctions en la remplaçant (taper dans `draw_func`) sans avoir de variable d'état. La fonction en cours d'exécution représentant l'état courant. C'est un aspect de la [programmation orientée prototype](https://fr.wikipedia.org/wiki/Programmation_orient%C3%A9e_prototype).

Bien sûr, avec une vtable, modifier un des pointeurs impacte tous les objets y faisant référence.

Après, rien n'empêche d'avoir un comportement hybride, une vtable pour les fonctions communes, peu utilisées ou "lentes", et un membre pour les fonctions polymorphiques qui doivent etre "rapides" d'accès ou qui changent en court de route. Des projets tels [dyno](https://github.com/ldionne/dyno) permettent cela.


## La méthode du gros switch

C'est une méthode très bourrin et pas très adapté au C++. Elle consiste à connaître l'ensemble des types utilisés pour une interface et de les associer à la fonction. Plus de vtable, plus d'indirection, mais à la place un saut à base de switch. Le compilateur pourrait plus facilement inliner le code, mais on perd la posibilité de charger dynamiquement des bibliothèques.

Le problème, c'est qu'il faut centraliser manuellement toutes les instances au même endroit pour faire notre table de saut, car il n'existe aucun moyen de récupérer -- par exemple -- toutes les instances d'un trait `is_drawable`. Si le compilateur ne nous aide pas, cette solution n'est pas facile à maintenir. On peut néanmoins passer par un générateur de source qui sélectionnerait à la manière d'un `clang-query` tous les types pour générer complètement le corps de la fonction `Drawable::draw`.
