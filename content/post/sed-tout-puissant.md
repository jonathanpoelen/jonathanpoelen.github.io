---
title: "Sed tout puissant"
#description: ""
date: 2012-06-11T10:11:50+01:00
#lastmod: 2018-02-13T02:36:50+01:00
#slug: "a-site-page"
#toc: false
#tags: [ "sed" ]
aliases: []
categories: [ "script shell" ]
draft: false
ghcommentid: 0
---

Il y a 3 semaines environ je cherchais le moyen d'utiliser la commande sed avec une regex sur plusieurs lignes. Je voulais transformer tous les `/\+\n\s+""/` en rien du tout (oui, les supprimer…). Sauf que comme tel, ça ne fonctionne pas, `sed` comme beaucoup de commandes unix fonctionne par ligne. Après de lourdes et pompeuses recherches d'au moins 7 secondes montre en main, je suis tombé sur la solution.

Pour ce faire, il suffit d'un identifiant, un petit label, une information de multi-ligne au milieu et 3 autres bricoles ; rien que ça :D.

Ce qui se traduit par:

```bash
sed -e :a -e N -e '$!ba' -e s'/+\n\s\+""//g'
```

Et en version courte:

```bash
sed ':a;N;$!ba;s/+\n\s\+""//g'
```

Je sais ce que vous vous demandez: pourquoi il n'y a pas `-e` dans la version courte ?

Mais parce que c'est la version courte, évidemment !

Sinon dans une moindre mesure.

- `:a`:
    pour créer un identifiant nommé "a", mais il peut très bien se nommer bidule.
- `N`:
    pour lire la ligne suivante et l'ajouter dans l'espace de recherche.
- `$!ba`:
    Si on n'est pas (`!`) sur la dernière ligne (`$`), sauter au label (`b`) nommé "a". C'est une boucle.

L'espace de recherche est la partie sur laquelle s'applique une regex. Par défaut, l'espace de recherche ne contient qu'une ligne à la fois qui sont manipulées individuellement. Avec cette boucle, l'espace de recherche contient toutes les lignes (avec saut de ligne) et la regex est appliquée sur l'ensemble du document.

Et en bonus, quelques commandes supplémentaires bons à savoir.

- {{<hi sh>}}sed 2,5s/e/U/g{{</hi>}} remplace `e` par `U` à partir de la ligne 2 jusqu'à la 5.
- {{<hi sh>}}sed "/$debut/,/$fin/"s/A/a/g{{</hi>}} depuis `$debut` jusqu'à `$fin`.
- {{<hi sh>}}sed "/$debut/,+5"s/A/a/g{{</hi>}} depuis `$debut` et sur 5 lignes.
