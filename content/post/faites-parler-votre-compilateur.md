---
title: "Faites parler votre compilateur"
#description: ""
date: 2018-02-13T03:00:28+01:00
#lastmod: 2018-02-13T03:00:28+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

En C++, notre meilleur ami est le compilateur. Encore faut-il bien le configurer pour qu'il nous crache un maximum d'avertissements en pleine poire.
Hélas, il s'avère que les options dépendent grandement du compilateur et de la version.

Du côté de Clang, il y a un `-Weverything` qui active absolument tous les warnings -- dont certains que je qualifie de douteux --,
alors que pour Gcc, `-Wall` et `-Wextra` n'activent pas tout.
Mais pour les 2, il n'y a pas d'option qui regroupe celle de débogage ou d'analyse dynamique.
Du coup, pour chaque nouvelle version, il y a un nombre plus ou moins important de flag à ajouter.

De ce constat, et parce que je possède plusieurs formats de fichier à mettre à jour, je me suis fait un générateur qu'on peut trouver sur mon github: [cpp-compiler-options](https://github.com/jonathanpoelen/cpp-compiler-options). Les fichiers générés sont dans le dossier `output` et couvrent clang-3.1 à 6.0 et gcc-4.7 à 7.1.

Petite astuce pour gcc et clang. On peut mettre des options dans un fichier et le donner au compilateur en préfixant le nom du fichier par `@`. Il y a un mémo dans le README.
