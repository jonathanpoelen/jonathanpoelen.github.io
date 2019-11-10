---
title: "Ma vision de l'accessibilité appliquée pour ce blog"
slug: "ma-vision-de-laccessibilite-appliquee-pour-ce-blog"
#description: ""
date: 2018-01-02T14:18:22+02:00
#lastmod: 2017-08-07T02:18:22+02:00
#tags: [ ]
categories: ["accessibilite-web"]
#project_url: ""
#slug: "a-site-page"
#toc: false
draft: false
ghcommentid: 0
expire: 2128
---

Pour changer de la programmation logicielle, je vais parler de l'accessibilité d'un site et des petits détails exaspérants que je rencontre sur la toile. Je pense qu'une bonne partie parle à chacun, généralement on fait avec, simplement parce qu'il n'y a pas d'autre choix, mais c'est toujours frustrant de tomber dessus.

Pour le blog, j'ai passé pas mal de temps sur un template déjà existant en touchant finalement un peu à toutes les parties CSS et HTML. Je vais ici parler un peu de couleur, de police, de lien, de la manière de disposer des cadres et quelques autres bricoles insignifiantes et par définition essentielles.

<!--{{< disclaimer >}}
Mon expérience dans le domaine se limite à ce blog et je n'ai pas la prétention d'être un pro du milieu. Ma manière d'aborder les problèmes peut être à l'antipode des bonnes pratiques, mais ce sont mes choix et, finalement, je les impose aux visiteurs comme le fait n'importe quel site :).
{{< /disclaimer >}}-->
<!-- c'est moâ qui décide :p !-->



## Plus de clics, moins de contenu

Depuis plusieurs années, une pratique courante consiste à multiplier les clics pour accéder à du contenu. Cela prend plusieurs formes, généralement à travers un menu sectionné sur une multitude de pages, des listes déroulantes ou du texte qui apparait au clic. De l'intuitif comme ils disent.

Le défaut de vouloir tout cacher est de rendre difficile d'accès l'information:

- Il n'est pas possible de faire une recherche de texte.
- Si on ne sait pas dans quel menu aller, on finit par cliquer partout.
- L'information est diffuse sur plusieurs pages, c'est beaucoup de manipulations, pénible et long. Encore plus s'il faut naviguer sur plusieurs pages avec une connexion lente.

Un des exemples typiques que je déteste est matérialisé par la [FAQ de dvp](https://cpp.developpez.com/faq/cpp/): chaque catégorie doit être déroulée pour qu'apparaissent les questions. Résultat, depuis la mise en place de ce nouveau système, je ne la consulte plus puisque trouver une question approximative est une gageure. Il me faut trouver la catégorie, l'éventuelle sous-catégorie, dérouler chacune et enfin pouvoir faire une recherche de texte sur le mot qui m'intéresse dans la question (le moteur de recherche donne souvent beaucoup trop de résultats).

À contrario, les FAQs sur le site du gouvernement permettent un affichage de toutes les questions et/ou réponses ce qui permet une recherche rapide et une lecture continue.

Je trouve que le site [doisjeutiliser.fr](https://doisjeutiliser.fr) donne un bon résumé de la situation. Et je suis d'accord avec tous les autres contre-exemples.

De plus, comme le langage graphique est différent entre les logiciels et entre sites on ne sait pas toujours le comportement associé au clic ou au lien par manque d'indicateur, on va au-devant de grosses surprises. Une icône qui représente une sous-catégorie déroulée pour tel site, mais enroulée pour un autre site. Un clic pour dérouler alors que cela ouvre une nouvelle page, voire pire, une nouvelle fenêtre ou, inversement, ouvrir dans un nouvel onglet un lien fait pour afficher un zone cachée (généralement, on tombe sur une page vide). Dans les 2 scénarios c'est une mauvaise surprise.

Et certains ont la bonne idée de redéfinir (ou de simuler) les actions d'une liste déroulante en supprimant toute action clavier. Tu veux atteindre "août" en appuyant sur `a` ? Fais autrement ! Avec un peu de chance, il faudra aussi cliquer sur le bouton "Ok" pour valider son choix. Il ne faudrait pas user la touche entrée...

Vous l'aurez compris, je suis contre cette pratique du "clic pour apparaître" à outrance.
Pour prendre en exemple le blog, sur l'ancien (blogspot) les archives sont déroulées par années et par mois. Avec une moyenne qui dépasse difficilement 2 articles/mois, en trouver un précis en moins de 2 clics mérite une médaille. Ici, les archives sont sur une page dédiée qui permet une vue d'ensemble immédiate sans étaler les titres sur plusieurs lignes.

(On y notera la police monospace sur la date qui facilite le parcours vertical ;).)


## Disposition générale et débordement

De mon point de vue, un site web possède classiquement un en-tête, un menu et du contenu, le tout positionné dans un nombre restreint de possibilités. La grande différence se joue sur la gestion du vide. Càd, la place que va prendre un élément sur la page.

Il existe principalement 3 cas de figures:

- Prendre un maximum de place.
- Mettre une limite maximum sur la largeur.
- Avoir une largeur fixée.

Une solution n'est pas meilleure qu'une autre, le choix dépend du design, du type de contenu et du dispositif d'affichage. La plupart du temps le site prend toute la largeur jusqu'à une taille raisonnable avant de permuter en taille fixe définie en pourcentage ou en pixel.

On peut très bien avoir un contenu qui prend toute la page quelle que soit la résolution, mais il faut être conscient qu'une ligne trop grande est difficile à lire. Plus une ligne est longue ou un paragraphe dense, moins il y a de points de repère visuels pour "suivre" le texte. Cela augmente le risque de glisser à la ligne suivante au milieu d'une phrase, de sauter une ligne ou relire la même au lieu de passer à la suivante. L'extrême inverse étant le format colonne utilisé par les journaux ou magazines, mais difficile à reproduire sur un navigateur.

Concernant la disposition du menu, à priori, n'importe laquelle fait l'affaire. À gauche, à droite ou en haut, quelle importance ? Mais c'est oublier un point essentiel: que faire quand un élément est trop grand ? La solution immédiate est simple: aller à la ligne. C'est même le comportement par défaut des navigateurs sur les blocs de texte. Ouf, nous sommes sauvés, tout va bien dans le meilleur des mondes.

Sauf qu'il existe aussi les images, les tableaux, les lecteurs vidéo et l'ensemble des éléments dont un "saut de ligne" n'est pas envisageable. Et là, c'est le drame. Heureusement, il existe plein d'horribles solutions.

Les exemples qui suivent sont faits avec une ligne de code qui ne fait pas de saut de ligne automatique.

- S'adapter au plus grand.
<!-- {{%fig "Une apparence sans défaut."%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/box-biggest-normal.png){{%/fig%}} -->
{{%fig "Débordement par la droite."%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/box-biggest-flex-row.png){{%/fig%}}
{{%fig "Débordement par la gauche. Le scroll disparaît. Au moins pour Firefox."%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/box-biggest-flex-row-reverse.png){{%/fig%}}
- Compacter les éléments ou mettre dans un scroll.
{{%fig "Un scroll alors qu'il y a de l'espace à droite."%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/box-scroll.png){{%/fig%}}
- Cacher le surplus.
{{%fig "Il n'y a plus moyen de lire le contenu qui déborde."%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/box-hidden.png){{%/fig%}}
- Superposer les éléments.
{{%fig "La partie droite cache une partie du contenu."%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/box-above.png){{%/fig%}}

Presque toutes ces images ont un point commun: un menu à droite. Ce n'est pas anodin, il est difficile d'avoir un menu à droite quand la partie gauche veut prendre sa place.

En déplaçant le menu à gauche, les problèmes de superposition contenu/menu disparaîssent d'eux-mêmes. On peut alors laisser le bloc prendre une place optimale.

Mais il faut le faire intelligemment pour ne pas tomber sur le premier cas qui ajoute un scroll horizontal pour l'ensemble du contenu. Vous vous imaginez devoir scroller pour lire une ligne ? Puis bon, les paragraphes qui deviennent des lignes de 3 kilomètres, non merci !

Il serait également bête de faire comme le second cas qui ajoute un scroll sur tous les éléments dépassant une certaine taille alors qu'un énorme vide persiste sur la droite.

J'ai assisté sur la doc de Hugo au passage d'un gabarit full page à celui d'une taille fixe et, malheureusement, chaque code se voyait estampillé d'un scroll horizontal. Obligé de déplacer la souris dessus pour qu'il s'agrandisse automatiquement jusqu'au bord droit de ma fenêtre. Ce qui rendait par la même occasion le scroll superflu. La seule question qui m'est venue est "Pourquoi dois-je utiliser une souris ? Pourquoi ce n'est pas l'apparence par défaut ?".

Bon ok, deux questions.

Pour le blog, mon choix s'est porté sur un menu à gauche, un contenu sur plus au moins 60% de large pour le centrer, mais avec les contenus trop grands qui débordent un maximum avant le recourt au scroll. C'est la disposition la plus pratique à mes yeux. Le résultat peut même être visible sur l'avant-dernier article dont un code possède une ligne plus grande qu'à l'habitude.


## Liens, zone de clic et surbrillance

Comment parler du web sans un petit mot sur les liens ? Le point central de la navigation internet et pourtant si difficile à cliquer pour moi.

J'ai l'impression de faire partie des gens dont la souris est invariablement attirée par la rangée de pixels non cliquable en plein milieu d'un lien. C'est systématique lorsque celui-ci est sur plusieurs lignes. Tout indique que je suis dessus: la couleur de fond du cadre qui change, celle du texte, la petite décoration qui apparaît. Tout, sauf le pointeur de souris. Mais il y a tellement de changement autour que ce détail ne se voit même plus.
<!-- Demande-moi de faire un gâteau, je te dirai que l'alarme incendie ne s'est pas déclenchée lorsque le four s'est enflammé. -->

{{%fig "Clic... Clic ? Clic clic clic ! Comment ? Je ne suis pas dessus ?"%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/menu-multi-line.png){{%/fig%}}

Arrive mon second fléau, déplacer suffisamment la souris pour être sur le lien. Mais pas trop, parce que ce foutu cadre qui change de couleur n'en fait pas partie non plus.

Le problème que je rencontre ici est récurant. Des changements de couleurs et de formes donnent l'impression de pouvoir effectuer une action alors que ce n'est pas vrai. Il vient alors un sentiment d'incompréhension très désagréable qui se change petit à petit en frustration lorsqu'on réalise que le pointeur de souris n'a pas la bonne forme.

Quand un cadre contenant un lien change de couleur au passage de la souris, c'est une invitation au clic. Celui-ci se doit d'être cliquable pour répondre au principe de la moindre surprise. Bonus, cela élargit la surface de clic et rend par la même occasion le lien plus accessible.

<!--{{%fig "Contre toute attente, la souris n'est pas sur le lien."%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/menu-hover-bad.png){{%/fig%}}
VS.-->
{{%fig "C'est quand même plus agréable lorsque le cadre est un lien."%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/menu-hover-good.png){{%/fig%}}

Cette extension de la zone de clic est très présente sur le blog. Comme les espaces blancs décoratifs sur le menu qui font partie du lien. Il est possible en plein écran de plaquer le curseur à gauche et de pouvoir cliquer dessus. Ou par exemple les liens "Précédent" et "Suivant" en fin d'article qui prennent chacun une moitié de largeur de contenu. Ou encore les icônes de flux RSS qui possèdent une zone de clic réelle beaucoup plus grande que l'image. Chacun de ces éléments possède une zone de clic plus large pour simplifier la visée.

Une dernière chose à éviter impérativement sur un lien concerne l'application d'un effet de gras au survol. Si par malheur le lien se situe en fin de ligne, la mise en gras va agrandir de quelques pixels les lettres et peut déplacer des mots à la ligne suivante. Qui se retrouvent en dehors de la zone de pointage. Qui reviennent car ne sont plus concernés par le survol. Puis repartent. Et ainsi de suite rendant le lien presque incliquable pour ceux ayant échappé à la crise d'épilepsie. Bien sûr, si le lien est une boîte qui grandit ou ne se déplace pas, il n'y a aucun problème.

À présent, parlons d'une évidence: un lien doit être mis en surbrillance par rapport au reste du contenu. S'il n'y a aucun repère visuel qui le différencie, personne ne le remarquera. Dans l'imaginaire collectif, un texte bleu ou violet et surtout souligné représente un lien. Bien que l'on puisse le modifier, je déconseille d'enlever le soulignement. Un simple changement de couleur pouvant être considéré comme une mise en gras.

En plus de la pseudo-classe `:hover` --source des problèmes cités au début du chapitre lorsqu'elle est mal utilisée--, il est essentiel de mettre en valeur les liens possédant le focus (`:focus`) pour les adeptes de la navigation clavier. Certains navigateurs comme Opéra possèdent des raccourcis clavier très pratiques pour sauter de lien en lien, mais très handicapants lorsque le site n'utilise pas ce marqueur visuel.

(Je profite du paragraphe pour parler des [liens d'évitement](https://www.alsacreations.com/article/lire/572-Les-liens-d-evitement.html#menu) ([ici](http://blog.atalan.fr/des-liens-devitement-astucieux/#categories) et [là](http://www.accede-web.com/notices/html-css-javascript/12-navigation-clavier/12-2-liens-evitement/)) pour sauter les regroupements de texte comme le menu et simplifier la vie des utilisateurs de clavier et autres dispositifs).

Pour finir sur les pseudo-classes et clore le sujet, je trouve que beaucoup de sites ignorent `:active`, alors que c'est un excellent moyen d'informer un visiteur que son clic est bien pris en compte. C'est un indicateur qui manque souvent quand j'ouvre dans un nouvel onglet: ai-je bien cliqué ? Cette information m'est tellement essentielle que je force dans mon navigateur un cadre en pointillé rouge lorsque je valide un lien.


## La police des caractères

Une chose que j'ai apprise en testant plusieurs polices d'écriture, est que la taille apparente varie énormément entre 2 fonts. Mettre une taille de police adaptée à la lecture n'est pas suffisant, il faut aussi que chaque police dans la liste de `font-family` ait une taille similaire au risque d'avoir des textes trop grands, ou pire, trop petits.

Je suis également tombé sur un conseil qui va à l'encontre du bon sens: mettre un `font-size: 62.5%` global. Avec pour seule justification que la taille par défaut est trop grande. Sans prendre en compte la taille effective d'une police et en rejetant toute configuration utilisateur. Si "62.5%" est trop petit, c'est le problème de l'utilisateur après tout.

<table>
  <tr><th>Police</th><th>16px</th><th>10px (62.5%)</th></tr>
  <tr style="font-family: sans-serif">
    <td>Sans-serif</td>
    <td style="font-size: 16px">Lorem ipsum</td>
    <td style="font-size: 10px">Lorem ipsum</td>
  </tr>
  <tr style="font-family: Arial">
    <td>Arial</td>
    <td style="font-size: 16px">Lorem ipsum</td>
    <td style="font-size: 10px">Lorem ipsum</td>
  </tr>
  <tr style="font-family: 'Times New Roman'">
    <td style="font-size: 16px">Times New Roman</td>
    <td style="font-size: 16px">Lorem ipsum</td>
    <td style="font-size: 10px">Lorem ipsum</td>
  </tr>
  <tr style="font-family: Times">
    <td style="font-size: 16px">Times</td>
    <td style="font-size: 16px">Lorem ipsum</td>
    <td style="font-size: 10px">Lorem ipsum</td>
  </tr>
</table>

Étrangement, il n'y a que sur internet où la taille est redéfinie. Sur une application de bureau personne ne s'en occupe et les polices et tailles configurées au niveau du système sont utilisées automatiquement pour les différents types d'éléments (titre, paragraphe, police monospace, etc).

Il est également possible d'utiliser les polices configurées au niveau du navigateur via leur [nom générique](https://developer.mozilla.org/en-US/docs/Web/CSS/font-family#Values), les principales étant `serif`, `sans-serif` et `monospace`.

Concernant l'[empattement](https://fr.wikipedia.org/wiki/Empattement_\(typographie\)) (serif ou sans-serif), j'en suis venu à la conclusion qu'une police sans empattement est plus lisible sur un écran. Peut-être à cause de la petitesse des lettres qui "charge" visuellement les glyphes. Ou Le rétro-éclairage, je ne sais pas.


## Le pouvoir de la couleur

La couleur est un vecteur d'information aussi importante --voire plus-- que la forme. Par contre, il faut utiliser la couleur d'usage sur l'objet représenté au risque de perturber le lecteur.

{{%fig "Perturbant, n'est-ce pas ?"%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/ok-cancel.png){{%/fig%}}

La couleur et les effets permettent également de distinguer les éléments entre eux. Par exemple, une légère ombre sur un bouton lui donne un relief qui le distingue d'un cadre lambda. Mais il faut savoir varier. Si je prends comme exemple les boutons sociaux, il est difficile d'en trouver un précisément lorsqu'ils sont tous de la même couleur.

{{%fig "Y a-t-il ton réseau préféré ?"%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/social-gray.png){{%/fig%}}

Alors que mettre la couleur habituellement utilisée par une marque permet de se focaliser presque sans effort dessus.

{{%fig "Et maintenant ?"%}}![](/post/ma-vision-de-l-accessibilite-applique-pour-ce-blog/social-color.png){{%/fig%}}

Même si les couleurs du site sont banales, je me permets quelques remarques. Premièrement, il vaut mieux se restreindre à un nombre raisonnable de couleurs, mais suffisamment contrastées pour les dissocier. Trop de couleurs empêchent de hiérarchiser l'information, un contraste trop faible de dissocier les éléments ce qui rend illisible le texte.

Il existe différents outils pour faire des palettes de couleurs qu'on peut fabriquer avec le logiciel [Agave](https://doc.ubuntu-fr.org/agave), le très bon site [Adobe Kuler](https://color.adobe.com/fr/create/color-wheel), [colourco.de](https://colourco.de) ou [code-couleur.com](http://www.code-couleur.com/) qui liste quelques palettes de couleurs. Ce dernier contient également la signification de certaines couleurs dans le monde occidental. Je trouve intéressant d'ajouter cet [article trouvé sur la toile](https://alfange.com/graphisme-10-couleurs-signification/) pour voir un peu --bien que difficile à vérifier-- l'évolution et usage au fil du temps. En cherchant un peu, on tombe sur pléthore de sites et de conseils. On a l'embarras du choix.

Si je peux donner un conseil, évitez les couleurs trop pures ; celles qui contiennent une composante de couleur proche de 255 et les autres à 0 comme pour les couleurs primaires (ex: `#ff0000`). Ce sont des couleurs très prononcées, pas du tout naturelles. Je trouve qu'elles brillent trop et agressent les yeux.

Une dernière chose concernant les couleurs, tout le monde ne les voit pas de la même façon, essayer un filtre pour [simuler le daltonisme](https://www.toptal.com/designers/colorfilter) est une expérience intéressante.


## Les métadonnées sont importantes !

Les métadonnées sont toutes les informations relatives à un document qui permettent notamment de le replacer dans un contexte sans le supposer après lecture de plusieurs paragraphes (même ainsi, certains éléments sont impossibles à déduire). Elles sont tellement importantes qu'il est possible de déduire le contenu du document associé sans l'avoir jamais lu.

J'exagère un peu, mais si on prend un article de blog et qu'on a l'habitude de lire les commentaires de l'auteur sur des forums, on se fait une relativement bonne idée du contenu de ses écrits. Mais pour cela il faut au moins avoir le nom de l'auteur, le sujet principal (titre, domaine du sujet) et probablement la date. Cela paraît évident, mais des fois, un ou plusieurs éléments manquent. Grossièrement, ce sont pour moi des marqueurs qui permettent de filtrer les lectures potentiellement intéressantes et de se préparer mentalement.

Plus généralement, les métadonnées sont un moyen de mettre en relation des données avec d'autres via des [URIs](https://fr.wikipedia.org/wiki/Uniform_Resource_Identifier) dans le but de faire des requêtes extrêmement sophistiquées à la manière de SQL ([SPARQL](https://fr.wikipedia.org/wiki/SPARQL)). On parle aussi de [web sémantique](https://fr.wikipedia.org/wiki/Web_sémantique) que je trouve une très bonne idée pour des ressources ayant beaucoup de relations, de dépendances, etc comme les encyclopédies ou des documents de normalisation, mais c'est un travail de longue haleine et franchement superflu dans le cadre d'un blog.

Donc voilà, les métadonnées c'est cool, mais je me contente de quelques informations en tête et pied d'article. On peut aussi les retrouver de manière plus structurée dans le [flux RSS](/index.xml).

Parmi les informations qui gravitent autour des articles ici, on trouve:

- Les catégories, car c'est un moyen facile de centrer le domaine d'application et filtrer les contenus.
- La date, parce qu'une information se périme vite. Les bonnes pratiques d'hier sont les mauvaises pratiques de demain. Et si je parle d'hier, on sait au moins de quel hier je fais référence.
- La date de dernière mise à jour s'il y a lieu. Il y a aussi une entrée dédiée dans le menu. Pour le moment je ne sais pas comment mettre en valeur les changements, peut-être une note en pied de page ?
- Le temps de lecture. C'est un bête calcul en fonction du nombre de mots qui peut être plus précis que la taille du scroll en présence d'image.
Et comme la page d'accueil contient plusieurs articles, la taille du scroll n'est pas une information fiable.
- L'auteur, même si pour le coup il n'y aura -- probablement -- que moi.
- Le lien permanent et les boutons de partage pour conquérir le monde.<!-- Comme chaque soir Minus.-->


## Les commentaires

Sur un site statique, il n'est pas possible d'intégrer son propre système de commentaire. On peut utiliser un service web comme [disqus.com](https://disqus.com) qui est très bien fait, mais aussi lourd et lent à charger.

C'est un reproche qui peut se faire sur beaucoup de sites. À force d'incorporer nombre de services externes (pour certains discutables), le site est ralenti et perd en fluidité.

Comme je pense qu'un espace commentaire est toujours intéressant, je tenais vraiment à en avoir un sur le blog. Alors je me suis dit, quitte à utiliser github.io, autant y aller jusqu'au bout et me servir des issues de github comme système d'échange. Ce n'est pas aussi bien intégré dans la page qu'un système natif (il faut passer par github pour poster), mais cela fait son job. Je perds par contre les commentaires anonymes puisqu'il faut un compte pour poster.


## Mais encore

À vrai dire, il y a pas mal de petites choses non citées qui m'agacent beaucoup (titre tronqué devenant trop court, animation trop lente, etc), mais ce sont des conseils qu'on peut retrouver assez facilement sur la toile.

J'ai davantage axé mes propos sur des détails que je trouve peu discutés. Tellement peu évoqués que j'ai l'impression d'être le seul concerné. Pour dire. Mais si cela permet de faire réfléchir, tant mieux.

Après, c'est bien possible que n'étant pas spécialement attiré par les technos web, un site spécialisé que je ne connais pas en parle mieux que moi. Tant mieux aussi.
