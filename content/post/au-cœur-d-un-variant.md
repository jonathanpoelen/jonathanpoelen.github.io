---
title: "Au cœur d'un variant"
#description: ""
date: 2017-08-08T00:45:55+02:00
#lastmod: 2017-08-08T00:45:55+02:00
#tags: [ ]
categories: [ "c++" ]
#project_url: ""
#slug: "a-site-page"
#toc: false
draft: true
ghcommentid: 0
---

choix de la zone de stockage:

- char[] / std::union_storage...
- union recursive
 - linéaire
 - arbre
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
