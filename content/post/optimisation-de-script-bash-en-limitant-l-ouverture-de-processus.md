---
title: "Optimisation de script bash en limitant l'ouverture de processus"
#description: ""
date: 2013-05-29T00:23:12+01:00
#lastmod: 2018-02-13T03:14:12+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "bash", "script-shell" ]
draft: false
ghcommentid: 0
expire: 2048
---

Une des choses qui prend du temps dans l'exécution d'un script shell est le nombre de programmes appelés et par conséquent, le nombre de processus créés.

Le meilleur moyen d'accélérer un script est de passer par les builtins et limiter les boucles ouverture/fermeture de programme.
En fait, dans certains cas, on pourrait avoir une commande qui lit sur l'entrée standard et retourne un résultat ; un peu comme `bc`.
C'est là que les coprocessus viennent à la rescousse :).

Un coprocessus est un sous-shell exécuté de façon asynchrone et fournissant les flux d'entrée/sortie. Ceux-ci sont accessibles via {{<hi sh "$COPROC[1]"/>}} et {{<hi sh "$COPROC[2]"/>}}.

Du coup, avec `bc`, la méthode est d'écrire dans un flux et lire dans l'autre. Comme la lecture est bloquante, le script va attendre que `bc` retourne le résultat.

Et voilà ! L'ouverture d'un programme est remplacé par une lecture/écriture.

Pour exemple, un petit programme qui lit un fichier contenant des opérations mathématiques et les affichent suivies du résultat.

```bash
#!/bin/bash
[ -z "$1" ] && echo "$0: file" >&2 && exit 1

#coprocessus de bc avec la sortie d'erreur redirigée vers la sortie standard
coproc bc 2>&1

while read l; do
  echo "$l" >&${COPROC[1]} # écrire dans le processus
  read result <&${COPROC[0]} # réception du résultat
  echo "$l = $result"
done < "$1"
```

Sans `coproc`, la boucle est plus simple, mais un processus sera ouvert pour chaque ligne du fichier, ce qui est particulièrement coûteux.

```bash
while read l; do
  echo -n "$l = "
  echo "$l" | bc
done < "$1"
```

À tester avec par exemple un fichier de calcul comme celui-ci:

```
2+3
2+3*9
23*9
23s*9
3*9
```
