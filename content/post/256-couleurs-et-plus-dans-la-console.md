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
expire: 2044
---

À chaque fois que je cherche des infos sur les couleurs je tombe toujours sur les trucs basiques. Mais j'ai récemment appris l'existence de 256 couleurs dans la console en tombant sur un [dépôt contenant un $LS_COLORS](https://github.com/trapd00r/LS_COLORS) particulièrement fourni.

En fait, il s'avère qu'en rajoutant extended dans la recherche "color shell" on puisse trouver [quelque(s) ressource(s)](http://misc.flogisoft.com/bash/tip_colors_and_formatting). J'aurais bien voulu y penser la semaine dernière, ça m'aurait évité de comprendre par tâtonnement...

Ce dernier lien ne contient pas tous les effets et le tableau de compatibilité est ancien. Par exemple, Konsole support dim color depuis quelques années et le mode 256 couleurs fonctionne sur rxvt.

Le fonctionnement des couleurs du terminal reposent sur les séquences ANSI qu'on peut aussi trouver sous l'appélation VT100 qui est devenu un standard de fait. Le principe est d'écrire certaines séquences de caractères que le terminal va interpréter comme des commandes VT100 pour -- entre autres -- mettre en couleur. Une commande inconnue ne fait rien.


## Utilisation

```bash
echo -e "\e[${FormatColor}m"
```

Le caractère {{<hi sh "\e"/>}} correspond à la touche Esc (échap). On peut aussi l'écrire en hexadécimal {{<hi sh "\x1b"/>}} ou en octal {{<hi sh "\033"/>}}.

Pour bash et `echo`, il faut utiliser l'option `-e` pour interpréter les séquences backslashés ou utiliser la forme {{<hi sh "$'\x1b'"/>}}.

{{<hi sh "${FormatColor}"/>}} correspond à un nombre expliqué dans les chapitres suivant et représentant une couleur de texte, de fond ou un effet. Plusieurs formats peuvent être mis en les séparant par des {{<hi sh ";"/>}} (point virgule).


## Style/effet de texte

Effet                       | Code | Code annulation
----------------------------|------|----------------
normal                      | 0    |
gras                        | 1    | 21
sombre **                   | 2    | 22
italique *                  | 3    | 23
souligné                    | 4    | 24
clignotant                  | 5    | 25
couleur texte/fond inversée | 7    | 27
caché **                    | 8    | 28
barré *                     | 9    | 29

Les effets avec une seule étoile ne sont pas supportés dans quelques rare terminaux et ceux avec 2 étoiles ont un support variable.

Le code 6 (clignotant rapide) n'étant pas supporté, il n'est pas listé ici.

À savoir que le code 0 (normal) supprime également toutes les couleurs.


## 8 couleurs

C'est le mode de couleur supporté par la majorité des terminaux. La couleur réellement affichée peut également être configurée au niveau du terminal lui-même ce qui explique pourquoi selon les environnements toutes les couleurs ne sont pas similaires.

Couleur     | Texte | Fond
------------|-------|-----
Noir        | 30    | 40
Rouge       | 31    | 41
Vert        | 32    | 42
Jaune       | 33    | 43
Bleu        | 34    | 44
Magenta     | 35    | 45
Cyan        | 36    | 46
Gris clair  | 37    | 47
Défaut      | 39    | 49

Les valeurs 38 et 48 représentent un mode étendu expliqué plus tard.


## 16 couleurs

Ce sont 8 couleurs supplémentaires accessibles pour certains terminaux. Les couleurs réelles derrière peuvent également être configurées dans le terminal.

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

C'est un mode étendu, certains vieux terminaux limitent à 88 couleurs, mais la plupart en supportent 256. Il s'utilise avec le triplet suivant:

- Texte: {{<hi sh "38;5;${x}"/>}}
- Fond: {{<hi sh "48;5;${x}"/>}}

Dont {{<hi sh "${x}"/>}} est à remplacer par un nombre allant de 0 à 255 inclus.


## TrueColor

Il est possible d'utiliser le classique RGB quand le terminal le permet. Beaucoup d'interfaces consoles ne le prennent pas bien en charge ou ne le détectent pas. Pour la seconde catégorie, il faut généralement activer une option.

- Texte: {{<hi sh "38;2;${r};${g};${b}"/>}}
- Fond: {{<hi sh "48;2;${r};${g};${b}"/>}}

Dont {{<hi sh "${r}"/>}}, {{<hi sh "${g}"/>}} et {{<hi sh "${b}"/>}} sont à remplacer par un nombre allant de 0 à 255 inclus.


## Appliquer la couleur seulement en mode interactif

La couleur est bien pratique en mode interactif, mais peut devenir gênant lorsqu'on enchaîne les grep et sed. La plupart des commandes ont une option `--color` qui prend `auto`, `always` et `never` pour forcer ou non l'utilisation des couleurs. `auto` permet de mettre la couleur uniquement si la sortie est un terminal. En bash, le test ce fait comme ceci:

```bash
if [[ -t 1 ]]; then
  echo stdout est un terminal
else
  echo stdout n\'est pas un terminal
fi
```

Il est ensuite facile de définir des variables qui contiennent les séquences ANSI correspondantes.

```bash
error=$'\e[31m'
reset=$'\e[0m'
if [[ ! -t 1 ]]; then
  error=
  reset=
fi

echo "${error}oups${reset}"
```

Exécuter le script va afficher "oups" en couleur, alors que faire quelque chose comme `./lescript.sh | cat` n'affichera plus de couleur puisque grâce au `|`, la sortie n'est plus un terminal, mais l'entrée de la commande `cat`.


## Notes de fin

Comme TrueColor et les couleurs étendues ne fonctionnent pas partout, le terminal peut interpréter la valeur des couleurs comme des commandes VT100 et détériorer le rendu final. Dans une telle situation, il faut le réinitialiser avec `tput reset`.

Voici un bout de code qui permet de visualiser une palette de 256 couleurs.

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
