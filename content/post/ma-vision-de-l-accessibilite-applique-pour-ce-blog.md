---
title: "Ma vision de l'accessibilité appliqué pour ce blog"
#description: ""
date: 2017-08-07T02:18:22+02:00
#lastmod: 2017-08-07T02:18:22+02:00
#tags: [ ]
#categories: [ ]
#project_url: ""
#slug: "a-site-page"
#toc: false
draft: true
ghcommentid: 0
---

En fesant le blog, je voulais qu'un visiteur sache rapidement l'arrivé d'un nouvel article et puisse le lire directement. Comme mon rythme de parution est relativement faible, je considère que cliquer partout pour avoir cette information est une perte de temps. C'est aussi pour cela qu'il y a un flux RSS général et par catégorie. On peut alimenter sont aggrégateur préféré et attendre devant, tout fébrille, une nouvelle publication.

La crétion du design m'a prit beaucoup plus de temps que prévus alors qu'il est basé sur une template proche des mes envies. En réalité, j'ai énormement touché aux détails: icone, police, couleur, lien, en-tête et pied de page des articles et page du site, etc. Un peu tout y est passé et il reste pourtant à faire.

Dans la suite je vais expliquer certains de mes choix et pourquoi, ainsi que les héritants que je rencontre sur divers sites.

{{< disclaimer >}}
Mon expérience dans le domaine se limite à ce blog et je n'ai pas la prétendu d'être un pro du milieu. Garder cela en mémoire en lisant la suite.
{{< /disclaimer >}}


## Disposé de l'information

Comme dit plus haut, ce que je trouve important en allant sur un site est **d'avoir rapidement l'information utile** et surtout, **scroller un minumum** et faire **le moins de clic possible** pour y accéder.

C'est bête, mais s'il me faut scroller pour lire ce qui m'interesse, je vais moins sur le site voir n'y vais plus. Dans le pire des cas, je ne remarque même pas que la page est scrollable. J'ai le même comportement si je considère trop important le nombre de clic à faire.

Dans le cadre d'un blog, les informations utiles sont les nouveaux articles depuis la dernière visite et les méta-données qui gravite autour. Parmit les celles importantes qui se trouvent dans l'en-tête d'un article on trouve:

- La date, parce qu'une information se périme vite. Les bonnes pratiques d'hier sont les mauvaises pratiques de demain. Dans l'avenir, je mettrais un bandeau d'avertissement automatique pour les articles trop vieux.
- La date de dernière mise à jour s'il y a lieu. Il y a aussi une entrée dédié dans le menu. Pour le moment je ne sais pas comment mettre en valeur les changements, peut-être une note en pied de page ?
- Les catégories, car c'est un moyen facile de centrer le domaine d'application et filtrer les contenus.
- Le temps de lecture, pour ménager son temps.

Il y aussi les liens pour accèder aux commentaires et partages sur les réseaux sociaux. Comme se sont des actions qui se font normalement en fin de lecture, celles-ci se trouvent en pied de page.

### Apparence Générale

Concernant le contenu textuel, il faut être conscient qu'une ligne trop grande est dificile à lire. Plus la ligne est longue et le paragraphe dense, moins il y a de point de repère pour "suivre" le texte. Cela augmente aussi le risque de sauter une ligne pendant la lecture ou quand les yeux reviennent sur la gauche. C'est une des raisons du format colonne qu'on trouve dans les journeaux. Par conséquent, la largeur du blog est fixé sur les résolutions trop importante.

Au moment où j'écris ces lignes, je n'ai pas modifié la position d'origine du menu, présent à doite dans le template utilisé. Cela parait anodin, mais le menu à droite pose un problème pour les codes, tableaux ou images trop grandes pour s'afficher dans la largeur du contenu. Avec un menu à gauche, il serait possible de faire déborder le contenu par la droite jusqu'au bord de l'écran s'il reste de la place.

<!-- image -->


## Liens, zone de clic et surbrilliance

Une chose que je trouve agaçant sur un PC concerne la souris et, par extension, la zone de clic. Il faut choisir entre vitesse de déplacement et précision ; plus la vitesse est rapide, moins le pointage sera précis et inversement.

Une manière de palier le problème est d'avoir de large zone de clic au détriment du nombre d'information afficher dans l'espace.

Une autre façon d'aborder le problème est de considérer les espaces vides comme une extension de la zone de clic. C'est par exemple le cas des lien "Précédent" et "Suivant" en fin d'article qui prennent chacun une moitié de largeurde contenu. Également dans le menu où, hors mi la ligne de séparation, l'espaces vide (padding) est cliquable, y les espaces gauche et doit. L'icone de flux RSS possède quand à lui une zone de clic réel plus grande que l'image pour simplifier la visé, notamment du tactile.

On peut aussi étendre la zone de clic au éléments autour comme sur les icones dans les en-tête et pied de page d'article.

Une chose à ne surtout pas oublier sur un lien: le mettre en évidence. À travers une couleur ou un soulignement par exemple, ne serait-ce que pour savoir qu'une partie du texte est cliquable car ce n'est pas toujours évident. C'est également très frustant de cliquer sur un lien sans faire exprès parce que rien n'en indiqué la précense (oui, des fois je clique dans le vide sans raisons).

Il existe un effet inverse tout aussi agaçant: un lien qui change de couleur en pointant sur la zone englobante, mais qui n'est pas cliquable parce que la souris n'est pas sur le texte. Dans ces conditions, le cadre devrait faire partie intégrande du lien.

<!-- image -->

En plus de la pseudo-classe `:hover`, il est essentiel de mettre en valeur les liens qui possèdent le focus (`:focus`) pour les utilisateurs de clavier, entre autres. Ne pas oublier d'utiliser `:active` pour informer que le clic est bien pris en compte. C'est un indicateur qui manque sur certains sites quand j'ouvre dans un nouvel onglet (ai-je bien cliqué ?).

En parlant de peudo-classe, `:visited` est une propriété interessante pour savoir sans cliquer, quel sont les liens déjà regardés. Je ne l'ai pas mit pour le menu, mais c'est une erreur.


## Polices

Une chose qui j'ai apprit en testant plusieurs police d'écriture, est que la taille varie énormement entre 2 polices. Mettre une taille de police adaptée à la lecture n'est pas suffisant, il faut aussi que chaque police dans la liste de `font-family` aient une taille similaire au risque d'avoir des textes trop grand, ou pire, trop petit.

<!-- image -->

J'ai pas mal réfléchit sur l'utlisation ou non de police avec [empattement](https://fr.wikipedia.org/wiki/Empattement_(typographie)) pour finalement adopter le sans empattement que je trouve plus lisible sur un écran.

<!-- TODO -->


## Couleurs

Il est évitant que des couleurs jaillit une atmosphère. Un site vantant les bienfait d'une eau pétilliante n'aura pas la même couleur dominante que celui d'un groupe de rock. À vrai dire, je trouve que les couleurs de mon blog sont plutôt fade... Je ne suis visiblement pas doué pour la décoration :'(.

Premièrement, il vaut mieux se restrainte à un nombre résonnable de couleur, mais suffisament constrasté pour les dissocier. Trop de couleur empêche de hiérarchiser l'information, un contraste trop faible de dissocier les éléments.

Il faut aussi savoir être cohérent et garder de préfèrence le même aspect pour les mêmes éléments d'une page tout en mettant en relief les éléments interactifs.

<!-- image -->

Il existe différent outils pour faire des palettes de couleur qu'on peut fabriquer avec le logiciel [Agave][agave], le très bon site [Adobe Kuler][adobe_kuler], [colourco.de/](http://colourco.de/) ou [code-couleur.com][http://www.code-couleur.com/] qui liste quelques palette de couleur. Ce dernier contient également la significaion de certaines couleurs dans le monde occidental. Je trouve interessant d'ajouter cet [article trouvé sur la toile](couleur_signification) pour voir un peu --bien que difficile à vérifier-- l'évolution et usage au fils du temps.

Pour le moment mon thème joue plutôt dans les cours du noir et blanc, mais il est en réalité plus compliqué de distinguer des éléments par leur apparence seul. Si je prend comme exemple les boutons sociaux, il est difficile d'en distingué un précisement.

<!-- image -->

Mettre une couleur habituellement utilisé par une marque permet de se focaliser sans effort dessus.

<!-- image avec les logo dans un autre ordre, et pourtant on le trouve rapidement -->

Pour la couleur et effet des liens à l'intérieur d'un article, j'ai opté pour le principe du moindre surprise: des couleur et un comportement proche de celle par défaut des navigateurs. Souligné et bleu clair, violet lorsqu'il est visité et un encadré rouge lors du clic. Je pense qu'un texte souligné avec une couleur différente est ancré dans l'imginaire collectif comme un lien.

Si je peux donner un conseil, évitez les couleurs trop pures ; celle qui contiennent un `ff` dans la valeur héxadécimal. Se sont des couleurs très prononcées, pas du tout naturelles. Elles brillent trop et agressent les yeux.

Une dernière chose concernant les couleurs, tous le monde ne les voient pas de la même façon, utiliser un filtre pour [simuler le daltonisme](https://www.toptal.com/designers/colorfilter) est une expérience interessante.


## Commentaires

Sur un site statique, il n'est pas possible d'intégrer son propre système de commentaire. On peut utiliser un service web comme [Disqus](disqus) qui est très bien fait, mais je le trouve lourd et lent à charger.

Quitte à utiliser github.io, autant y aller jusqu'au bout et me servir des issues de github comme système d'échange. Ce n'est pas aussi bien intégré dans la page (il faut passer par github pour poster), mais cela fait son job.


<!-- -->

effet: couleur pour les informations utile mais dissocié du contenu
en-tête petite
focus et hover sur les liens (cadre, surprise avec du multiligne et une souris au centre qui ne peut pas cliquer, lien clignotant car effet de grossisement, on ne sais pas que c'est un lien)
commentaire avec github plutot que disqus (poid, temps de chargement)
bouton et fond
liens et images
font serif/sans, taille des polices et différence de taille entre police (choisir des polices de même taille)
flux rss complet et par categorie
lien externe iconifié (pas encore fait)

note: rendre le bouton de la catégorie cliquable

note: en-tête pour vieil article: prendre du recul, l'article date de plus de 2ans

note: pas d;info visuel sur le menu pour les articles déjà lu. Ajouter une icone/texte ou non ?

:active même si pas hover

[adobe_kuler]:https://color.adobe.com/fr/create/color-wheel
[colourco]:http://colourco.de/
[agave]:https://doc.ubuntu-fr.org/agave
[couleur_signification]:https://alfange.com/graphisme-10-couleurs-signification/
[disqus]:https://disqus.com
