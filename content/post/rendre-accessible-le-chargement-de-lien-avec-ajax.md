---
title: "Rendre accessible le chargement de lien avec AJAX"
#description: ""
date: 2011-12-31T12:05:43+01:00
#lastmod: 2018-02-13T02:05:43+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "accessibilite-web", "javascript" ]
draft: false
ghcommentid: 0
---

Le but d'un site chargeant ces pages avec AJAX est double:

- Diminuer la charge du serveur en ne demandant que le contenu à modifier (ce qui peut potentiellement réduire le nombre de requêtes à la base de données et les données transférées).
- Avoir une navigation plus fluide et réactive puisque le navigateur travaille moins.

Toutefois, si mal fait, des problèmes apparaissent au niveau de l'historique qui n'est plus utilisable. Par la même occasion, il sera impossible d'accéder à une page via son url. Quant à ceux n'ayant pas javascript ‑moteur de recherche compris‑ il est totalement impossible de visiter le site.

Le premier problème peut se corriger avec [`history.pushState`](https://developer.mozilla.org/en-US/docs/Web/API/History_API#The_pushState().C2.A0method) qui permet d'ajouter, supprimer et modifier l'historique d'une page. Le second problème est tout simple, il suffit d'indiquer une page complète et ajouter un paramètre dans l'url pour les utilisateurs ayant javascript. Si ce paramètre existe, la page ne renvoie que le contenu désiré. Dans le cas contraire, la page est entièrement retournée.

Par exemple avec un lien sur `ma_page.php` qui retourne une page complète. On intercepte l'événement click. On ajoute un paramètre, par exemple `ajax`, ce qui donne `ma_page.php?ajax` puis on charge cette page. Le serveur ne renvoie que le contenu, on ajoute `ma_page.php` dans l'historique et on remplace l'ancien contenu par le nouveau.

Il faut également vérifier que les liens fonctionnent lorsqu'ils sont ouverts dans un nouvel onglet. Il n'y a rien de plus énervant pour moi qu'un lien qui ne peut pas s'ouvrir dans un autre onglet. J'ai pour habitude en lisant un article ou certains sites d'ouvrir les liens potentiellement intéressants dans un nouvel onglet. Ceux s'ouvrant en javascript sont une vraie plaie ! Ils ouvrent une page blanche et quelle horreur de constater au bout d'un quart d'heure que toutes mes pages dûment sélectionnées sont ouvertes sur le néant.

Alors soit je fuis le site, soit je commence un véritable parcours du combattant pour l'ouvrir dans un onglet:

- copier l'url (Ctrl+L, Ctrl+C)
- retour en arrière (Alt+gauche)
- ouvrir un nouvel onglet (Ctrl+T)
- aller sur la page copiée (Ctrl+Shift+V ou Ctrl+V, Entrée)
- revenir sur l'onglet original (Ctrl+Shift+Tab)

Généralement, je fuis !

Pour une bonne expérience utilisateur, il faut aussi indiquer les chargements quelque part, surtout quand ils s'éternisent (c'est perturbant de ne rien voir réagir après un clic). Et ne surtout pas oublier la gestion d'erreur. Avoir des problèmes de connexion peut arriver à n'importe qui, si le site ne réagit plus sans en avertir l'utilisateur, alors dans la tête de l'usager, le site est bogué. Mais il faut le faire de manière intelligente, pas en cachant le contenu sous une énorme popup ;).
