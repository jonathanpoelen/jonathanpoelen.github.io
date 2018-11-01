---
title: "Zsh et le danger des modificateurs"
#description: ""
date: 2012-10-14T19:46:36+01:00
#lastmod: 2018-02-13T02:32:36+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "script-shell", "zsh" ]
draft: false
ghcommentid: 0
expire: 2048
---

Zsh est très bien comme shell, mais fait plus de choses que bash ce qui peut engendrer des bugs quand celui-ci est le shell par défaut et que des scripts ne définissent pas l'interpréteur utilisé. J'ai eu le coup une fois lorsqu'il fallait charger le module `canberra-gtk` pour les programmes java.

Une variable `$GTK_MODULES` est définie et contient tous les modules gtk séparés par des deux points (`:`).
Visiblement, j'ai 2 modules gtk. Tous 2 servant à ajouter `canberra-gtk-module`. Les fautifs ? `52libcanberra-gtk-module_add-to-gtk-modules` et `52libcanberra-gtk3-module_add-to-gtk-modules` Avec un contenu identique:

```bash
if [ -z "$GTK_MODULES" ] ; then
    GTK_MODULES="canberra-gtk-module"
else
    GTK_MODULES="$GTK_MODULES:canberra-gtk-module"
fi

export GTK_MODULES
```

Et là, paf ! C'est le drame. `$GTK_MODULES` = `canberra-gtk-moduleanberra-gtk-module`, naturellement :D.

Vous venez d'assister à l'effet du modificateur `c` (dans `:c`) qui ajoute le chemin de l'executable présent dans `$GTK_MODULES`. Comme `canberra-gtk-module` n'est pas un executable, rien n'est fait, mais `:c` disparaît. Avec l'auto-complétion on peut voir qu'il y en a une petite quinzaine de disponibles, de quoi tomber plusieurs fois dans le piège.

Les modificateurs existent aussi dans bash, mais ils sont moins nombreux et ne s'utilisent qu'avec l'historique (paragraphe `HISTORY EXPANSION` du manuel).

```bash
echo unfichier.txt
echo !:1:r
```

Résultat :

```
unfichier.txt
unfichier
```

Quasiment jamais utilisé en fait.

Mais voilà, zsh les généralise aussi aux variables ce qui engendre une erreur dans `GTK_MODULES`.

Alors comment faire ?
Simple, entourer la variable d'accolades:

```bash
GTK_MODULES="${GTK_MODULES}:canberra-gtk-module"
```
