---
title: "Différence entre $@, $*, \"$@\" et \"$*\""
#description: ""
date: 2013-02-17T10:42:27+01:00
#lastmod: 2018-02-13T02:55:27+01:00
slug: "difference-entre-variables-speciales"
#toc: false
#tags: [ ]
aliases: []
categories: [ "script-shell" ]
draft: false
ghcommentid: 0
expire: 2058
---

Dans un script shell, il existe 2 variables pour accéder aux paramètres de la commande (aussi nommées `argv` dans pas mal d'autres langages): {{<hi sh "$*"/>}} et {{<hi sh "$@"/>}}.

- {{<hi sh "$*"/>}} est une variable ce qu'il y a de plus normale et ne diffère pas d'une autre variable. Cependant, le comportement des variables diffère en fonction du shell, notamment sur zsh (j'y reviens après).
- {{<hi sh "$@"/>}} est une variable au comportement différent entre les shells basés sur sh (bash, dash, ...) et les autres (ksh, zsh).

Sur bash, il n'y a aucune différence entre {{<hi sh "$*"/>}} et {{<hi sh "$@"/>}}. Par contre, sur ksh/zsh, {{<hi sh "$@"/>}} représente un tableau d'arguments. Il faut savoir que sur bash, utiliser une variable sans guillemet revient à créer autant d'arguments qu'il y a de mots. Les mots sont séparés en fonction des caractères de {{<hi sh "$IFS"/>}} (la variable contient : espace, tabulation et saut de ligne). C'est pour cela qu'il est conseillé d'entourer ces variables de guillemets doubles.

Un petit exemple pour comprendre :).

```bash
a='a b c'
for v in $a ; do
  echo $v
done
```

```
> a
> b
> c
```

Le résultat est 3 lignes pour 1 paramètre, la chaîne `'a b c'` contenant des espaces s'est fait couper. Ce n'est pas le cas pour zsh qui ne fait pas cette césure (raison historique, j'y reviens à la fin).

Maintenant il reste {{<hi sh "\"$*\""/>}} et {{<hi sh "\"$@\""/>}} qui ne diffère pas entre les shells.

- {{<hi sh "\"$*\""/>}} correspond à une seule chaîne, tout est géré en un seul bloc.
- {{<hi sh "\"$@\""/>}} représente les paramètres réels. C'est identique à {{<hi sh "$@"/>}} avec ksh et zsh.


## Démonstration

```bash
# test.sh
for v in XXX ; do
  echo $v
done
```

Le code ci-dessus est utilisé avec {{<hi sh "./test.s 'a b' c"/>}} et pour lequel `XXX` est remplacé par une des 4 variables précédemment citées.

XXX                   | bash        | ksh         | zsh
----------------------|-------------|-------------|-------------
{{<hi sh "$*"/>}}     | a<br>b<br>c | a<br>b<br>c | a b<br>c
{{<hi sh "$@"/>}}     | a<br>b<br>c | a b<br>c    | a b<br>c
{{<hi sh "\"$*\""/>}} | a b c       | a b c       | a b c
{{<hi sh "\"$@\""/>}} | a b<br>c    | a b<br>c    | a b<br>c

Pour que zsh boucle sur des mots, il faut ajouter un flag à la variable ({{<hi zsh "${=*}"/>}}), comme ci-dessous.

```zsh
a='a b c';
for v in ${=a} ; do
  echo $v
done
```
