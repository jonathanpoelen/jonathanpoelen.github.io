---
title: "Grep, sed, awk, sort... Non ! Zsh"
#thumbnail: ""
#description: ""
date: 2019-01-28T22:45:03+01:00
#lastmod: 2019-01-26T02:45:03+01:00
slug: "grep-sed-awk-sort...-non-!-zsh"
#toc: false
#tags: [ ]
aliases: []
categories: [ "zsh", "script shell" ]
draft: false
ghcommentid: 0
expire: 2028
---

Depuis plusieurs années maintenant, j'utilise Zsh comme shell par défaut. Et par la force des choses, il m'arrive de taper des commandes zsh, de faire des boucles zsh, de penser zsh. Bref, de coder en zsh. Bien que le langage a des inconvénients, il possède de nombreuses fonctionnalités qui recouvrent celles de certains utilitaires Unix.

Les gros avantage d'utiliser zsh plutôt que les commandes Unix sont au nombre de 3:

- Pas de post traitement pour mettre le résultat d'une commande dans une variable.
- Beaucoup plus rapide que lancer une commande. La création de process prend du temps et se ressent dans une boucle.
- Écrire en 3 lettres ce qu'on peut faire en 32. C'est-à-dire frimer ;).

Il y a aussi des inconvénients:

- Moins lisible, surtout lorsque l'on remplace une suite de pipe comme {{<hi zsh "aaa | bbb | ccc"/>}}. Mais on peut simplifier avec des variables intermédiaires.
- Peut-être plus lent lorsqu'il y a beaucoup de texte à manipuler. En gros, faire un grep d'un fichier de 1000 lignes est plus lent qu'avec zsh, mais plus rapide si le fichier fait 100000 lignes, car zsh ne travaille pas par flux.


## Syntaxe de base

Le `man` de zsh fait plus de 5 fois celui de bash. Le manuel est tellement gros qu'il est divisé en 16 parties + un {{<hi zsh "man zshall"/>}} pour les afficher tous. Difficile de tout mettre en 1 article alors je me contente de certaines parties de `zshexpn` (zsh expansion and substitution). Parmi celles utilisées ici il y a les options d'expansion de paramètres ({{<hi zsh "${(@)x}"/>}}) et les modificateurs ({{<hi zsh "${x:h}"/>}} ou {{<hi zsh "$x:h"/>}}). Il existe un pdf compilant les options et syntaxe de zsh: [http://www.bash2zsh.com/zsh_refcard/refcard.pdf](http://www.bash2zsh.com/zsh_refcard/refcard.pdf).


### Lire un fichier

Voici comment les zsh peuvent lire un fichier et mettre chaque ligne dans un tableau grâce aux extensions de paramètres:

```zsh
contents=$(<file) # lire un fichier
contents=$(<file1 <file2) # lire 2 fichiers
contents=$(<file ; ls) # lire un fichier et le retour de commande `ls`

lines=( ${(f)contents} ) # tableau sans ligne vide
lines=( ${(s:\n:)contents} ) # équivalent
lines=( "${(@f)contents}" ) # tableau avec toutes les lignes (il faut les quotes et @)

lines=( ${(f)$(<file)} ) # forme condensée du premier cas

echo ${(j:\n:)lines} # concatène les lignes avec \n
# ou
echo ${(F)lines}

# affiche chaque ligne d'un fichier dans des crochets
echo "[${(j:]\n[:)"${(@f)$(<file)}"}]"
```

Il y a encore de nombreux paramètres qui peuvent être trouvés dans le manuel ou via l'auto-complétion de zsh. Aussi, pour simplifier les exemples qui suivront, j'utiliserai directement les variables `contents` et `lines`.


### Glob et glob étendu

L'une des grandes forces de zsh réside dans le globbing. Il ne se restreint pas qu'à la recherche de fichier, mais peut aussi s'appliquer sur les éléments d'un tableau ou des chaînes pour filtrer ou transformer. En plus de `*` et `?`, zsh comprend `[...]`, `[^...]` et `<x-y>` pour un nombre entre `x` et `y` inclu (`<->` pour n'importe quel digit).

Avec le glob étendu (`setopt extendedglob`) on possède alors un équivalent des regex:

- `x#` 0 ou plus de `x`
- `x##` 1 ou plus de `x`
- `x~y` exclut `y` de `x`
- `^x` tous sauf `x`
- `(#s)` début (équivalent de `^` des regex)
- `(#e)` fin (équivalent de `$` des regex)

Ainsi que la syntaxe ksh si activée

ksh-like | glob operators
---------|----------------
{{<hi zsh "@(...)"/>}} | {{<hi zsh "(...)"/>}}
{{<hi zsh "*(...)"/>}} | {{<hi zsh "(...)#"/>}}
{{<hi zsh "+(...)"/>}} | {{<hi zsh "(...)##"/>}}
{{<hi zsh "?(...)"/>}} | {{<hi zsh "(|...)"/>}}
{{<hi zsh "!(...)"/>}} | {{<hi zsh "(^(...))"/>}}


## Équivalent des commandes \*Unix

Maintenant que la petite introduction syntaxique est faite, on peut s'attaquer au remplacement des commandes systèmes. Bien sûr, toutes les options d'une commande ne peuvent pas être simulées facilement avec zsh, mais je présente ici l'essentiel. Je précise que les commandes bash ont implicitement {{<hi zsh "<<<$contents"/>}} comme flux de lecture et que le résultat des commandes zsh est fait avec un `echo`.

Je conseille aussi le petit [Zsh Native Scripting Guide](https://github.com/zdharma/Zsh-100-Commits-Club/blob/master/Zsh-Native-Scripting-Handbook.adoc).


### grep

bash | zsh
-----|----
{{<hi zsh "grep 'Alligator'"/>}} | {{<hi zsh "${(M)lines:#*Alligator*}"/>}}
{{<hi zsh "grep -v 'Alligator'"/>}} | {{<hi zsh "${lines:#*Alligator*}"/>}}
{{<hi zsh "grep '^Alligator .* Alligator$'"/>}} | {{<hi zsh "${(M)lines:#Alligator * Alligator}"/>}}
{{<hi zsh "grep -i 'alligator'"/>}} | {{<hi zsh "${(M)lines:#(#i)*alligator*}"/>}}
{{<hi zsh "grep -m1 'aligator'"/>}} | {{<hi zsh "${lines[(r)*aligator*]}"/>}}

`(#i)` n'est utilisable qu'avec `setopt extendedglob` et peut s'appliquer sur un groupe seulement de caractère (i.e. `((#i)a)lbator`). Il existe l'option inverse: `#I`. Ainsi que `#l` qui fait une recherche insensible à la case pour les lettres minuscules du pattern, et en majuscule pour celles en majuscule dans le pattern.


### agrep

`agrep` pour "approximate grep". C'est un `grep` qui autorise une marge d'erreur dans la recherche. Zsh possède une option de glob qui fait à peu près la même chose: `(#a3)` pour une recherche avec 3 erreurs.

bash | zsh
-----|----
{{<hi zsh "agrep -3 'alligator'"/>}} | {{<hi zsh "${(M)lines:#*((#a3)alligator)*}"/>}}


### sed

bash | zsh
-----|----
{{<hi zsh "sed '3,6!d'"/>}} | {{<hi zsh "$lines[3,5]"/>}}
{{<hi zsh "sed s/aligator/crocodile/"/>}}  | {{<hi zsh "${contents/aligator/crocodile}"/>}}
{{<hi zsh "sed s/aligator/crocodile/g"/>}} | {{<hi zsh "${contents//aligator/crocodile}"/>}}
{{<hi zsh "sed 's/^aligator\*$/_/'"/>}}   | {{<hi zsh "${lines:s%aligator*%_}"/>}} ou {{<hi zsh "${lines/(#s)aligator\*(#e)/_}"/>}}
{{<hi zsh "sed 's/^\w\+$/[&]/'"/>}} | {{<hi zsh "${lines:/(#m)[[:alnum:]]##/[$MATCH]}"/>}}
{{<hi zsh "sed -E 's/^(\w+) = (\w+)$/\2 = \1/'"/>}} | {{<hi zsh "${lines:/(#b)([[:alnum:]]##) = ([[:alnum:]]##)/$match[2] = $match[1]}"/>}}


### head

bash | zsh
-----|----
{{<hi zsh "head -n3"/>}} | {{<hi zsh "$lines[1,3]"/>}}


### awk

Pour celui-là, l'entrée sera le texte ci-dessous. Le programme va coloriser le préfixe.

```
info: un alligator dort sur le balcon
avertissement: l'alligator se réveille
erreur: l'alligator attaque
note: penser à investir dans une porte plus solide
```

awk | zsh
-----|----
{{<hi awk>}}BEGIN {
  colors["erreur:"]="31"
  colors["avertissement:"]="33"
  colors["info:"]="35"
  for (k in colors)
    colorized[k]="\033[" colors[k] "m" k "\033[0m"
}
{
  s=colorized[$1]
  if (s)
    print s substr($0, length($1)+1)
  else
    print $0
}{{</hi>}} | {{<hi zsh>}}declare -A colors=(erreur: 31 avertissement: 33 info: 35)
declare -A colorized
for k in ${(k)colors} ;
  colorized+=($k "\033[$colors[$k]m$k\033[0m")

echo ${(F)lines/(#m)(#s)[a-z]##:/${colorized[$MATCH]:-$MATCH}}{{</hi>}}

### find

bash | zsh
-----|----
{{<hi zsh "find -name '*aligator*'"/>}} | {{<hi zsh "**/*aligator*"/>}}
{{<hi zsh "find -name '*aligator*' -a -not '*crocodile*'"/>}} | {{<hi zsh "**/*aligator*~*crocodile*"/>}}
{{<hi zsh "find -type d"/>}} | {{<hi zsh "**/*(/)"/>}}
{{<hi zsh "find -not -type d"/>}} | {{<hi zsh "**/*(^/)"/>}}
{{<hi zsh "find -type l"/>}} | {{<hi zsh "**/*(@)"/>}}
{{<hi zsh "find -atime 3"/>}} | {{<hi zsh "**/*(a3)"/>}}

Bon, vous l'aurez compris, zsh permet de nombreux filtres dans la recherche de fichier. Il peut trier la recherche par date, nom, groupe, etc ou même via une fonction personnalisée.


### sort

bash | zsh
-----|----
{{<hi zsh "sort"/>}} | {{<hi zsh "${(o)lines}"/>}}
{{<hi zsh "sort -n"/>}} | {{<hi zsh "${(on)lines}"/>}}
{{<hi zsh "sort -rn"/>}} | {{<hi zsh "${(On)lines}"/>}}


### Manipulation de chemin

bash | zsh
-----|----
{{<hi zsh>}}dirname "$0"{{</hi>}} | {{<hi zsh "$0:h"/>}}
{{<hi zsh>}}basename "$0"{{</hi>}} | {{<hi zsh "$0:t"/>}}
{{<hi zsh>}}realpath "$0"{{</hi>}} | {{<hi zsh "$0:P"/>}}

Il y en a évidemment d'autres.


### cut

bash | zsh
-----|----
{{<hi zsh "cut -d: -f2,1"/>}} | {{<hi zsh "${lines/(#m)*/$(() { echo $2:$1 } ${(s:b:)MATCH})}"/>}}

Mais une boucle serait mieux ici.


### printf

bash | zsh
-----|----
{{<hi zsh "printf '%04d' 42"/>}} | {{<hi zsh "echo ${(l:4::0:)${:-42}}"/>}} ou {{<hi zsh "echo ${(l:4::0:)$n}"/>}}
