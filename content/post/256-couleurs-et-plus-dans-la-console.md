---
title: "256 couleurs et plus dans la console"
#description: ""
date: 2014-06-09T00:35:19+01:00
#lastmod: 2018-02-13T03:36:19+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "script-shell" ]
draft: false
ghcommentid: 0
---

À chaque fois que je cherche des infos sur les couleurs je tombe toujours sur les trucs basiques. Mais j'ai récemment appris l'existence de 256 couleurs dans la console en tombant sur <a href="http://dotshare.it/dots/100/">un screen</a> un peu trop de coloré. Ce qui m'a dirigé sur un <a href="https://github.com/trapd00r/LS_COLORS">dépôt contenant un $LS_COLORS</a> particulièrement fourni.</p>

En fait, il s'avère qu'en rajoutant extended dans la recherche "color shell" on puisse trouver <a href="http://misc.flogisoft.com/bash/tip_colors_and_formatting">quelque(s) ressource(s)</a>. J'aurais bien voulu y penser la semaine dernière, ça m'aurait évité de comprendre par tâtonnement...

## Utilisation

```bash
"\e[${FormatColor}m"
```

Le caractère {{<hi sh "\e"/>}} correspond à la touche Esc (échap). On peut aussi l'écrire en héxadécimal {{<hi sh "\x1B"/>}} ou en octal {{<hi sh "\033"/>}}.

Pour bash et `echo`, il faut utiliser l'option `-e` pour interpréter les séquences backslashés.

{{<hi sh "${FormatColor}"/>}} correspond à un nombre représentant une couleur de texte, de fond ou un effet. Plusieurs format peuvent être mis en les séparant par des {{<hi sh ";"/>}} (point virgule). L'ordre n'a pas d'importance.

## 8 couleurs

C'est le mode de couleur supporté par la majorité des terminaux. La valeur des couleurs peut également être configuré.</p>

Couleur | Texte | Fond
--------|-------|-----
Noir    | 30    | 40
Rouge   | 31    | 41
Vert    | 32    | 42
Jaune   | 33    | 43
Bleu    | 34    | 44
Magenta | 35    | 45
Cyan    | 36    | 46
Gris clair | 37 | 47

## Style/effet de texte

L'italique (3) n'est pas répertorié dans le lien précédemment cité. Aussi, l'effet sombre (ou dim) et caché ne fonctionnent pas partout. Il n'existe pas de code 6.

Effet      | Code | Code annulation
-----------|------|----------------
normal     | 0    |
gras       | 1    | 21
sombre     | 2    | 22
italique   | 3    | 23
souligné   | 4    | 24
clignotant | 5    | 25
inversé    | 7    | 27
caché      | 8    | 28

## 16 couleurs

Ce sont 8 couleurs supplémentaire accessible pour certains terminaux.

Couleur       | Texte | Fond
--------------|-------|-----
Gris foncé    | 90    | 100
Rouge clair   | 91    | 101
Vert clair    | 92    | 102
Jaune clair   | 93    | 103
Bleu clair    | 94    | 104
Magenta clair | 95    | 105
Cyan clair    | 96    | 106
Blanc         | 97    | 107

## 88 et 256 couleurs

C'est un mode étendue, certains vieux terminaux limitent à 88 couleurs, mais la plupart en supportent 256. Il s'utilise avec le triplet suivant:

- Texte: {{<hi sh "38;5;${X}"/>}}
- Fond: {{<hi sh "48;5;${X}"/>}}

Dont {{<hi sh "${X}"/>}} est à remplacer par un nombre allant de 0 à 255 inclus.

## TrueColor

Il est possible d'utiliser le classique RGB quand le terminal le permet. Beaucoup d'interface console ne le prennent pas bien en charge ou le ne le détecte pas. Pour la seconde catégorie, il faut généralement activer une option.

- Texte: {{<hi sh "38;2;${r}${g}${b}"/>}}
- Fond: {{<hi sh "48;2;${r}${g}${b}"/>}}

Dont {{<hi sh "${r}"/>}}, {{<hi sh "${g}"/>}} et {{<hi sh "${b}"/>}} sont à remplacer par un nombre allant de 0 à 255 inclus.

## Notes de fin

Outre le fait que TrueColor et les couleurs étendues ne fonctionnent pas partout, elles peuvent allègrement pourrir le rendu. Pour retrouver un rendu normal utiser `tput reset`.

Le bout de code ci-dessous permet de visualiser une palette de 256 couleurs.

```bash
for m in 38 48 ; do
  n=0
  for l in {0..31} ; do
    for c in {0..7} ; do
      echo -ne "\033[$m;5;$n;1m$n\e[0m\t"
      ((++n))
    done
    echo
  done
done
```
