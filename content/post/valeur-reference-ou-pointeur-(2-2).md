---
title: "Valeur, référence ou pointeur ? (2/2)"
#description: ""
date: 2014-05-31T21:18:10+01:00
#lastmod: 2018-02-13T03:34:10+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

Dans le [précédent billet]({{<ref "valeur-reference-ou-pointeur-(1-2).md">}}), j'opposai les paramètres par références constantes à ceux par valeurs.

Sans plus attendre entamons la seconde question.

## Référence ou pointeur ?

Si je dois faire court je dirai: pointeur jamais ; référence quand possible. Sans autre forme de procès :D

Mais on me dit dans l'oreillette qu'il faut argumenter... Alors c'est parti.

Les références possèdent un contrat beaucoup plus fort que les pointeurs: elles ne peuvent être nulles et référencent toujours la même variable.

À contrario, les pointeurs peuvent changer la variable référencée ou ne pointer sur aucune variable ([nullptr](http://en.cppreference.com/w/cpp/types/nullptr_t).

Une référence est l'équivalent d'un pointeur constant non-nul (avec une syntaxe d'utilisation plus simple: pas besoin de déréférencer).
De leurs restrictions, celles-ci ne peuvent pas toujours correspondre au besoin ; les pointeurs sont alors envisageables.

De plus, les pointeurs sont beaucoup utilisés dans les constructions dynamiques (allocation dynamique) quand les classes sont à [sémantique d'entités](http://blog.emmanueldeloget.com/index.php?post/2011/11/18/Standard-C11-%3A-la-s%C3%A9mantique-de-d%C3%A9placement). Principalement car ces dernières ne sont pas copiables et que l'allocation dynamique permet de s'affranchir de la portée (le scope) en se détachant de la pile.


## Le pointeur parle trop

L'usage de pointeur (pointeur nu) est cependant à prendre avec des pincettes, voici 3 questions que soulève l'usage d'un pointeur:

- Dois-je contrôler la durée de vie du pointeur (le détruire) ? \[oui, non]
- Est-ce un élément ou une séquence d'élément ? \[séquence, simple valeur, ça dépend]
- Le pointeur peut-il être nul ? \[oui, non]

Après un petit calcul combinatoire (2*3*2), il y a 12 réponses possibles. Le pire est de répondre: "ça dépend". Si on l'enlève, il reste quand même 8 possibilités.

La sémantique du pointeur est, au final, très faible. Lui en ajouter devient alors capital.

## Plus de sémantique pour un pointeur

Hélas, il n'y a pas de réponse universelle, tout dépend des cas d'usages. De plus certaines combinaisons son conceptuellement douteuses.
Pour exemple, un pointeur non-nul mais qu'on détruira. Le non-nul amène aux références mais idéologiquement une référence n'est pas faite pour être détruite.

On peut néanmoins sortir quelques règles:

- Si le pointeur n'est pas nul et que l'appelé ne gère pas la durée de vie: `std::reference_wrapper` qui permet de changer la référence utilisée.
- Si l'appelé contrôle la durée de vie: pointeurs intelligents (`std::unique_ptr` en priorité, std::shared_ptr, ...).
- Si le pointeur peut être nul et que l'appelé ne gère pas la durée de vie alors un pointeur est "justifié". En interne du moins, pour l'extérieur un `non_owner_ptr` ou un `observer_ptr` sera plus parlant. Si le pointeur peut être invalidé pendant l'exécution alors `std::weak_ptr` ou autres est à envisager.
- Tout ce qui est tableau est indiqué dans les signatures des objets wrapper (`unique_ptr<T[]>`) ou/et grâce à un attribut de taille. De plus, s'il faut soit des tableaux, soit une valeur alors toujours préférer le type commun: tableau (les valeurs deviennent des tableaux de taille 1). Les tableaux dynamiques sont, quant à eux, plus faciles à utiliser avec `std::vector`.

Au final, l'usage de pointeur nu est très peu utilisé, voire pas du tout. De plus, leur mauvais usage avec l'allocation dynamique amène des fuites mémoires principalement dues aux libérations manuelles. Dans un langage comme le C++, un code non exception safe **va** faire des fuites mémoires. De manière générale, la libération s'applique sur toute forme de ressource: lock, fichier, etc.

Pour éviter cela, les ressources doivent être attachées à la pile et la puissance du déterminisme de destruction permettra de les libérer convenablement. On parle aussi de [RAII](http://fr.wikipedia.org/wiki/RAII). Pour rappel, tout ce qui est sur la pile est détruit à la sortie du scope. La sémantique de déplacement permettra de changer de portée.

Le wiki de Guillaume Belz en parle très bien: [pourquoi le RAII est fondamental en C++ ?](http://guillaume.belz.free.fr/doku.php?id=pourquoi_le_raii_est_fondamental_en_c)
