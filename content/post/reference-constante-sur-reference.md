---
title: "Référence constante sur référence"
#description: ""
date: 2013-05-13T03:18:42+01:00
#lastmod: 2013-05-15T20:00:42+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

Une petite note sur les références et le qualificatif {{<hi cpp "const"/>}} en commençant par un exemple :).

```cpp
using reference = int&;
int i = 0;
reference const r = i;
r = 3;
```

Contrairement à ce que laisse croire le code, il est possible de modifier la valeur de `r`.
En fait, ajouter {{<hi cpp "const"/>}} sur une référence ne fait rien car cela ne s'applique pas sur le référé qui reste un {{<hi cpp "int&"/>}}.

Si on déroule complétement le type de `r`, on obtient {{<hi cpp "int & const r"/>}}: une référence constante sur un entier.
Comme une référence est par nature immutable (le référé ne peut pas changer), le {{<hi cpp "const"/>}} est superflux.
Le type de `r` est donc bien une référence sur un entier non-constant.

(On notera toutefois que la syntaxe {{<hi cpp "int & const"/>}} est invalide puisque {{<hi cpp "const"/>}} ne peut s'appliquer directement sur une référence.)
