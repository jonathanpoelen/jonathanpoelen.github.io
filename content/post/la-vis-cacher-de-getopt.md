---
title: "La vis cacher de getopt"
#description: ""
date: 2012-12-25T12:58:55+01:00
#lastmod: 2018-02-13T02:45:55+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "script shell" ]
draft: false
ghcommentid: 0
---

Voici une petite information très mal connue et peu utilisée du `getopt` de la lib C et de la commande shell. Ainsi que `Boost.Program_options`. Parce que boost c'est bien :p.

Les noms des options longues n'ont pas besoin d'être écrites entièrement.

```sh
function parsecmd()
{
  getopt -o '' --long \
    option-longue,option-encore-plus-longue,une-autre-option: \
    -n 'example' -- "$@"
}

parsecmd --option-l --u plop bidule
```

Donne

```
--option-longue --une-autre-option 'plop' -- 'bidule'
```

La commande shell `getopt` est un peu plus souple que les autres. Si ambiguïté, la première option correspondante sera sélectionnée. Si l'option `-a` existe ce n'est plus le cas et le code d'erreur `1` est retourné ainsi qu'un petit message listant les options possibles. Mais avec `-a` les options longues peuvent commencer par un simple tiret.

La plupart des commandes Linux utilisant `getopt`, cette astuce peut s'utiliser assez souvent.
