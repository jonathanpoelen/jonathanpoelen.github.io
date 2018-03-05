---
title: "Délégation d'événement en js"
#description: ""
date: 2013-01-18T15:16:12+01:00
#lastmod: 2018-02-13T02:53:12+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "javascript" ]
draft: false
ghcommentid: 0
---

En règle générale les événements sont attachés à l'élément qui va le traiter.

Par exemple en javascript, un menu (ul) contenant 10 entrées (li) où chaque entrée est associée à une action crée, au total, 10 événements. 10 événements attachés à la même action.

Pour dire vrai, cette méthode est peu performante et peut la plupart du temps être remplacée par un seul événement sur le parent. À ce moment le parent vérifie si l'événement est généré par un de ces fils et fait le traitement en conséquence.

```js
var ul = document.getElementById("menu");
ul.onclick = function(e){
  if (e.target.parentNode === ul) //est-ce toi mon fils ?
    openWithAjax(e)
}
```

On peut trouver le même type de fonction dans JQuery: [.on()](https://api.jquery.com/on/).
