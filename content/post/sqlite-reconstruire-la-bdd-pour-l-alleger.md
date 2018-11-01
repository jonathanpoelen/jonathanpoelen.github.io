---
title: "Sqlite, reconstruire la bdd pour l'alléger"
slug: "sqlite-reconstruire-la-bdd-pour-lalleger"
#description: ""
date: 2013-03-04T23:36:14+01:00
#lastmod: 2018-02-13T03:02:14+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "SQLite" ]
draft: false
ghcommentid: 0
expire: 2038
---

Il existe sur les bases de données SQLite une commande pour réduire la fragmentation des tables et optimiser l'espace disque.
J'ai nommé: [VACUUM](http://sqlite.org/lang_vacuum.html).

Cette commande reconstruit la bdd pour éliminer les lignes vides et réorganise les index (plus de détails dans la doc en lien).

Comme certains logiciels se servent de sqlite comme BDD, il peut être intéressant d'utiliser cette commande de temps en temps.
La première fois que je l'ai fait pour firefox (fichier `~/.mozilla/firefox/nom-du-profil/*.sqlite` sur linux) j'ai gagné ½ giga :).

Voici un petit script qui va permettre de le faire sur tous les fichiers sqlite du système (du moins, ceux indexés) et connaître la taille totale avant et après utilisation:

```bash
#!/bin/sh
tmpf=${TMPDIR:=/tmp}/sqlite_file_path
locate \.sqlite \
| xargs -d'\n' mimetype \
| grep 'application/x-sqlite3$' \
| sed 's/:\s*application\/x-sqlite3\s*$//' \
> "$tmpf"
xargs --arg-file "$tmpf" -d'\n' du -hc | tail -n1
while read f ; do
  sqlite3 "$f" 'VACUUM;' || echo "\tfor $f"
done < "$tmpf"
xargs --arg-file "$tmpf" -d'\n' du -hc | tail -n1
rm "$tmpf"
```

Et une petite version qui prend des fichiers en paramètre:

```bash
#!/bin/sh
du -hc "$@"
for f in "$@" ; do
  sqlite3 "$f" 'VACUUM;'
done
du -hc "$@"
```
