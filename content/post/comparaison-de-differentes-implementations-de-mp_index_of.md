---
title: "Comparaison de différentes implémentations de mp_index_of"
slug: "comparaison-de-differentes-implementations-de-mp_index_of"
#description: ""
date: 2018-10-01T18:28:21+02:00
#lastmod: 2018-09-05T01:28:21+02:00
#toc: false
#tags: [ "méta-programmation" ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
image: "post/mp_index_of/image.png"
expire: 2028
---

Dans l'article précédent sur les variants, j'ai fait une implémentation un peu spéciale de `index_of`. Je vais présenter une quinzaine d'implémentations possibles et le coût de chacune sur le compilateur.

{{%info%}}L'implémentation citée précédemment ne se retrouve pas ici car une forme récursive plus "classique" a les mêmes conséquences.{{%/info%}}


## Avant-propos

Toutes les implémentations de `mp_index_of<T, Ts...>` retournent un `std::integral_constant<int, i>` correspondant à l'indice de `T` dans `Ts` ou `-1` si `T` n'existe pas.

Clang possède des outils tel que [templight](http://plc.inf.elte.hu/templight/), mais il n'y a pas d'équivalent pour Gcc. Par conséquent, les tests sont faits avec `/usr/bin/time --format="%E %M"` qui ressort le temps et la mémoire maximum utilisée par un programme.

Au niveau des options de compilation, `-fsyntax-only` permet de vérifier la validité d'un code sans générer de fichier de sortie. `-std=c++17` est présent pour utiliser les plies, mais tous les algorithmes n'en ont pas besoin. `-ftemplate-depth=n` et `-fconstexpr-depth=n` appliquent une profondeur limite dans la récursivité appliquée respectivement aux templates et aux fonctions constexpr. `-w` supprime tous les avertissements (tels que des macros non utilisés) pour éviter que des sorties inopportunes influencent la mesure.

Il y a 2 formes de test:

- ceux sur des listes contenant 2 types nommés `x` et `_` et
- ceux sur des suites `i<n>` avec `n` un entier et `x`.

Pour chaque test, `T` représente `x` et il peut être présent ou non dans `Ts`.

les valeurs de `Ts` sont présentées dans les légendes sous forme abrégée:

- `[x*500]`: une liste de 500 `x`.
- `[_*250, x, _*249]`: une liste de 250 `_`, suivit de `x` et d'une liste de `249` `_`.
- `[i{0..40}*150]`: uns suite de 150 `i0`, une autre de 150 `i1` et ainsi de suite jusqu'à `i40`.
- `[i0..i{0..140}]`: une suite `[i0..i0]`, puis `[i0..i1]`, etc jusqu'à `[i0..i140]`.
- `[i{0..n},x,i{n-(n..0)..n}];n=100`: les suites `[i{0..100},x,i{100..100}]`,  `[i{1..100},x,i{99..100}]`, etc

Les sources se trouvent sur {{<urlhi "mp_index_of" "github">}} dans un même fichier cpp. Chacune des implémentations possède son propre namespace et des macros activent individuellement chaque test. <!--TODO dossier des graphes + zip/tar.gz-->


## Relation temps de compilation et mémoire

Dans du code utilisant la méta-programmation, le temps de compilation suit la même courbe que la mémoire. Plus une compilation est lente, plus la mémoire utilisée augmente allant jusqu'à saturation.

Et la mémoire est fortement liée au nombre de types instanciés par le compilateur. Ici, il ne faut pas voir une instance comme une variable, mais comme la première création d'un type. Par exemple, `index_of<int, int>` est une instance de type (ou d'alias), `index_of<int, int, int>` une seconde instance, `index_of<int, float>` une troisième, etc. Chaque type construit s'ajoute dans la mémoire du compilateur et l'ensemble utilise de plus en plus de place. C'est ainsi que le compilateur implémente naturellement la [mémoïsation](https://fr.wikipedia.org/wiki/M%C3%A9mo%C3%AFsation) pour les templates.

Pour réduire le temps de compilation, il faut donc réduire le nombre de type instancié.


## La récursivité

La première chose qu'on apprend avec les variadiques, c'est que les boucles se font avec de la récursivité en enlevant un par un les éléments. C'est une forme très facile à écrire et à comprendre, mais qui crée beaucoup de types intermédiaires à l'intérieur du compilateur.

{{<fhi "mp_index_of/mp_index_of1.cpp" recursive_ternary>}}

Une implémentation avec ternaire récursive comme on peut la trouver dans le fichier variant de libstdc++. Elle souffre d'un léger problème: il y a autant d'instance de `_index_of` que d'élément dans la liste, même si `T` se trouve en première position.

{{<fhi "mp_index_of/mp_index_of1.cpp" recursive_indexed>}}

L'indice courant se trouve en paramètre template et la récursion s'arrête dès que `T` est trouvé. Seulement, l'ajout de l'indice en paramètre va dégrader la mémoïsation en créant plus de types que nécessaire.

{{<fhi "mp_index_of/mp_index_of1.cpp" recursive>}}

Un mélange des 2 implémentations précédentes sans les inconvénients. En réalité, c'est la forme la plus communément écrite.

<!-- ./gen_graphs.py compiler=g++ algo=recursive,recursive_indexed,recursive_ternary legend='~[];~all;~[_*1400];~[_*700, x*700];~n*' -- rec_gcc -->
![duré de compilation avec gcc de 3 implémentations récursives](/post/mp_index_of/rec_gcc_time.png)

Sans surprise, sur les 3 premiers tests, la version avec ternaire est à peu près stable quelle que soit la position de `x`, même s'il semble qu'évaluer la ternaire ralentisse un peu la compilation lorsque `x` se situe vers la fin.
Alors que les 2 autres implémentations sont beaucoup plus rapides lorsque `x` est au début pour rattraper progressivement la version ternaire. Sans que je sache l'expliquer, la version avec index explose le compteur pour le test 3.

Comme la courbe de progression pour la version récursive classique n'est pas linéaire, je déduis que le nombre d'éléments dans un pack d'argument influence le temps de compilation. Il y a ±600ms de différence pour parcourir les 700 premiers `_` contre seulement ±220ms pour les 700 restant. Cela peut se comprendre assez facilement, plus il y a de types dans la template, plus la création d'un identifiant au sein du compilateur va prendre du temps.

Le test 5 met clairement à défaut la version avec indice, car la mémoïsation ne s'applique pas ici. Avec la version ternaire et récursive, lorsque le compilateur calcule la position d'une liste de la forme `[0,1,2,3]`, il l'a aussi fait pour `[1,2,3]`, `[2,3]` et `[3]` ce qui permet d'être très rapide lorsqu'il tombe sur des valeurs déjà évaluées. Or, la version avec indice ajoute un élément qui empêche cela: `<0, [0,1,2,3]>` se déroule en `<1, [1,2,3]>`, `<2, [2,3]>` et `<3, [3]>` alors que `<0, [0,1,2]>` se déroule en `<1, [1,2]>` et `<2, [2]>`. Les suites sont différentes, le compilateur crée donc beaucoup plus de types intermédiaires ce qui ralentit la compilation.

Dans l'ensemble, l'algorithme avec index est à éviter, car il montre qu'une mauvaise mémoïsation ralentit fortement l'évaluation de template. Il faut également éviter la version ternaire au profil d'une implémentation qui stoppe la récursivité pour ne pas faire de calcul inutile. Finalement, l'algorithme de récursion classique est plus rapide dans tous les cas.


Ce qui est vrai pour Gcc, ne l'est pas forcément pour Clang comme le montre le graphique ci-dessous.

<!-- ./gen_graphs.py compiler=g++ algo=recursive,recursive_indexed,recursive_ternary legend='~[];~all;~[_*1200];~[_*600, x*600];~n*' -- rec_clang -->
![duré de compilation avec clang de 3 implémentations récursives](/post/mp_index_of/rec_clang_time.png)

Premier constat, le test 3 n'a pas de pic inexplicable pour l'algorithme avec indice. Et, bien que la version récursive classique soit toujours plus rapide que celle avec ternaire, il est amusant de constater que l'implémentation avec indice supplante de peu la version classique. Sauf bien sûr lorsque la mémoïsation est mise à défaut comme dans le test 5.

Si on compare le temps de compilation entre Clang et Gcc, il s'avère que Gcc est un peu plus rapide que son concurrent alors même que les premiers tests se font sur une liste de 1200 éléments contre 1400. De mon expérience, c'est presque toujours le cas avec des templates récursives sur de "grands" ensembles.

{{%info%}}La limite de 1200 est dûe à une limitation de Clang qui ne peut pas dépasser 1293 niveaux de profondeur sans crasher (en tout cas chez moi probablement à cause d'un dépassement de pile).{{%/info%}}


## Profiter davantage de mémoïsation

Il est possible d'augmenter la mémoïsation en réduisant les types dans la liste en une suite de valeur `true`/`false`. La mémoïsation est alors beaucoup plus efficace, mais la comparaison se fait sur chaque élément de la liste. Heureusement, dans un code normal, la probabilité de tomber sur la même instance de `std::is_same<T, U>` est très grande ce qui rend cette solution encore plus efficace.

{{<fhi "mp_index_of/mp_index_of1.cpp" recursive_bool>}}

L'utilisation de `typename std::is_same<T, Ts>::type` au lieu de simplement `std::is_same<T, Ts>` permet de restreindre les types à `std::false_type` et `std::true_type`.

On peut aussi faire la comparaison avec un `detail::_index_of` spécialement dédié à la recherche d'un `std::true_type`.

{{<fhi "mp_index_of/mp_index_of1.cpp" recursive_bool2>}}

Et une autre version qui travaille sur des valeurs plutôt que des types pour vérifier si cela a une influence.

{{<fhi "mp_index_of/mp_index_of1.cpp" recursive_bool3>}}

<!-- ./gen_graphs.py compiler=g++ algo=recursive,recursive_bool\? legend='~[];~all;~[_*1400];~[_*700, x*700];~n*' -- rec_bool_gcc -->
![comparaison de temps de 3 implémentations qui font une transformation en booléan](/post/mp_index_of/rec_bool_gcc_time.png)

Là où la version récursive était rapide, la transformation en une liste de booléan l'est tout autant. Le bénéfice devient évident sur une grande variété de listes (test 4) et des listes homogènes (test 6) où le temps de compilation est réduit par 3.

L'algorithme `recursive_bool2` spécialisé dans la recherche d'un `std::true_type` permet de grappiller encore quelques millisecondes principalement grâce à une spécialisation de template en moins.

À contrario, la version 3 qui manipule des booléens plutôt que des types est lente. Manipuler des types serait donc plus rapide que manipuler des valeurs dans les spécialisations. Le même comportement est présent dans Clang, même s'il est moins prononcé.

<!-- ./gen_graphs.py compiler=clang++ algo=recursive,recursive_bool\? legend='~[];~all;~[_*1200];~[_*600, x*600];~n*' -- rec_bool_clang -->
![comparaison de temps de 3 implémentations qui font une transformation en booléan](/post/mp_index_of/rec_bool_clang_time.png)


## Récursivité par bloc

La récursivité possède un défaut qui n'est pas forcément un problème dans le code de tous les jours: le niveau de récursion est limité par le compilateur. Gcc s'arrête à une profondeur de 900, Clang à 1024 et si je ne me trompe pas, la limite est de 500 pour MSVC. Bien sûr, cela est configurable. C'est d'ailleurs ce qui a été fait pour générer les premiers graphiques même si comme dit précédemment, Clang crashe lorsqu'on dépasse 1293.

Une technique employée pour dépasser artificiellement cette limite est de prendre plusieurs valeurs par niveau de récursivité. Cela réduit la profondeur de récursion et le nombre de types instanciés. S'il y a moins de types instanciés, la compilation devrait également être plus rapide.

Le code devient malheureusement beaucoup plus verbeux et compliqué:

{{<fhi "mp_index_of/mp_index_of1.cpp" pack8>}}

Et L'équivalent qui fait une transformation en liste de booléen avant:

{{<fhi "mp_index_of/mp_index_of1.cpp" pack8_bool>}}

<!-- ./gen_graphs.py compiler=g++ algo=recursive_bool2,pack8,pack8_bool legend='~[];~all;~[_*1400];~[_*700, x*700];~n*' -- pack8_gcc -->
![duré de compilation avec gcc pour un algo récursive par pack de 8](/post/mp_index_of/pack8_gcc_time.png)

Même lorsque la position de `x` est éloignée (test 2 et 3), les `pack8` sont efficaces. Si on prend la différence entre le test 1 et 2, et 2 et 3, il y a plus ou moins un facteur 8.

Comme c'était le cas lorsqu'on comparaît l'algorithme `recursive` avec `recursive_bool`, `pack8_bool` est plus rapide que `pack8` lorsque le nombre de listes différentes augmente (test 5 et 7).

Le test 6 profite déjà d'une mémoïsation optimale pour les 3 implémentations, même si `pack8_bool` est un chouia plus lent que les autres ici.

Toutefois, la suprématie de `pack8_bool` par rapport à `pack8` est beaucoup plus ténue avec Clang.

<!-- ./gen_graphs.py compiler=clang++ algo=recursive_bool2,pack8,pack8_bool legend='~[];~all;~[_*1200];~[_*600, x*600];~n*' -- pack8_clang -->
![duré de compilation avec gcc pour un algo récursive par pack de 8](/post/mp_index_of/pack8_clang_time.png)


## Penser différemment

Plutôt qu'avoir un `index_of` le plus rapide possible, il est possible de combiner d'autres algorithmes pour arriver au même résultat. Il y a un double avantage dans le cadre d'une bibliothèque de méta-programmation:

- Implémenter un nouvel algorithme efficace est plus facile, car les optimisations telles que celles utilisées par `pack8` sont déportées sur ceux avec une réelle implémentation.
- Un nombre restreint d'algorithmes est plus souvent utilisé ce qui augmente la mémoïsation.

Ainsi, `index_of` peut se faire en combinant `std::make_index_sequence`, `join`, `front` et éventuellement `tranform`. On peut aussi le faire avec `foldl`/`foldr`.

Pour le faire avec `join`, on construit une liste d'indices de la taille de `Ts` qui est ensuite transformée en `list<i>` lorsque le type est identique à `T` et en `list<>` dans le cas contraire. Puis, on fusionne toutes les listes en une seule avec un `int<-1>` ajouté en fin. Le premier élément de cette liste correspond au résultat de `index_of`.

Il serait préférable de ne pas ajouter `-1`, mais ajouter une valeur dans la liste puis faire un post traitement. Cela augmente les collisions de type et par conséquent la mémoïsation. Mais restons simple !

{{<fhi "mp_index_of/mp_index_of1.cpp" by_indices>}}

Wooa, cette monstruosité. Il faut être complètement barge pour écrire cette horreur. C'est pourtant une chose que l'on trouve dans [Metal](https://github.com/brunocodutra/metal/blob/master/include/metal/list/join.hpp#L51), [Brigand](https://github.com/edouarda/brigand/blob/master/include/brigand/sequences/append.hpp#L29), [Cpp11](https://github.com/boostorg/mp11/blob/develop/include/boost/mp11/detail/mp_append.hpp#L48) ou [Boost.Hana](https://github.com/boostorg/hana/blob/master/include/boost/hana/detail/variadic/foldl1.hpp#L23). La palme d'or revient à [Kvasir.Mpl](https://github.com/kvasir-io/mpl/blob/development/src/kvasir/mpl/sequence/join.hpp#L14) avec une template de 1025 éléments variadiques.

<!-- ./gen_graphs.py compiler=g++ algo=recursive,pack8_bool,by_indices legend='~[];~all;~[_*1400];~[_*700, x*700]' -- by_indices -->
![comparaison de temps avec gcc de pack8 et une qui utilise d'autres algorithmes](/post/mp_index_of/by_indices_time.png)

C'est étonnamment efficace. Pas autant que `pack8_bool`, mais globalement plus qu'une récursion classique. Dans un projet, `join` et `front` seront utilisés ailleurs et à travers d'autres algorithmes ce qui, toujours grâce à la mémoïsation, réduit le coût total.


## Fonction constexpr

Depuis C++14, il est possible d'avoir des conditions et des boucles dans les fonctions. On peut alors écrire `index_of` presque comme on écrirait n'importe quelle fonction impérative.

{{<fhi "mp_index_of/mp_index_of1.cpp" loop>}}

Les booléens sont en paramètre template. C'est dommage, parce que cela limite les transferts de paramètre depuis une autre fonction. Il vaut mieux mettre les valeurs en paramètre de fonction comme on le fait d'habitude.

{{<fhi "mp_index_of/mp_index_of1.cpp" loop2>}}

C'est beau, c'est simple, c'est élégant.

<!-- ./gen_graphs.py compiler=g++ algo=pack8_bool,loop\? legend='~[];~all;~[_*1400];~[_*700, x*700];~n*' -- func -->
![comparaison de temps de pack8_bool et une fonction constexpr](/post/mp_index_of/func_time.png)

C'est fou, mais dans l'ensemble on y gagne encore. Néanmoins, la version qui prend les valeurs en paramètre est légèrement plus lente.

J'ai aussi essayé avec des fonctions récursives, mais c'est une vraie catastrophe. Premièrement, c'est extrêmement lent. Deuxièmement, la profondeur de récursion est encore plus basse qu'avec les templates: 512 pour Gcc (900 avec templates) et 800 pour Clang (1024 avec template). Mais surtout, une profondeur trop grande fait rapidement planter Clang.

En réalité, le coût d'appel de fonction est supérieur à celui d'une instanciation de classe. Mais si la fonction n'est pas récursive, alors le coût peut être amorti par rapport à une implémentation récursive, comme ici.


## Fold expression

En C++17 arrive les fold expressions. Plus besoin de boucle explicite ou de fonction récursive pour dérouler des valeurs. Seulement, avec `index_of`, il faut jouer d'ingéniosité pour stopper le fold tout en ayant un compteur qui s'incrémente. La manière la plus simple est de mettre plusieurs instructions séparées par des `,`. La dernière valeur de cette "liste" sera celle utilisée pour vérifier si `T` égale `Ts`.

{{<fhi "mp_index_of/mp_index_of1.cpp" fold>}}

Le code est beaucoup plus concis, mais aussi beaucoup plus obscur. Il faut dire que `index_of` n'est pas le meilleur algorithme pour représenter la beauté des folds ;).

<!-- ./gen_graphs.py compiler=g++ algo=loop2,fold legend='~[];~all;~[_*1400];~[_*700, x*700]' -- fold_gcc -->
![comparaison de temps avec gcc d'une fonction constexpr et un fold](/post/mp_index_of/fold_gcc_time.png)

Le résultat est très similaire à une boucle manuelle. Clang donne la même chose, mais les versions précédentes de Gcc sont extrêmement lentes avec les fold.

<!-- ./gen_graphs.py compiler=g++-7 algo=loop2,fold legend='~[];~all;~[_*1400];~[_*700, x*700]' -- fold_clang -->
![comparaison de temps avec clang d'une fonction constexpr et un fold](/post/mp_index_of/fold_gcc_7_time.png)


## Pour terminer

On peut retenir de ces différents tests que

- le nombre de spécialisations a un coût,
- le temps de compilation est quadratique au nombre de paramètre variadique,
- il faut limiter la récursion et le nombre de types instanciés,
- les algorithmes doivent profiter de la mémoïsation,
- spécialiser des valeurs est plus lent que spécialiser des types,
- les fold expressions sont très efficaces,
- une fonction constexpr est lente à instancier, mais l'utilisation de boucle peut compenser ce défaut.

Il n'y a pas eu de test sur l'affirmation qui va suivre, mais d'expérience, je sais que les alias sont plus rapides qu'une instanciation de classe, mais aussi plus cryptique au niveau des erreurs.

Si vous voulez des graphiques qui comparent 2 compilateurs, le générateur de graphe et les résultats de compilation se trouvent dans {{<urlhi "mp_index_of" "le dépôt">}}.
