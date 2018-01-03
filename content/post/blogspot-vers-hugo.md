---
title: "De Blogspot à Hugo, il a changé de peau"
#description: ""
date: 2017-07-29T16:03:53+02:00
#lastmod: 2017-07-29T16:03:53+02:00
#tags: [ ]
#categories: [ ]
#project_url: ""
#slug: "a-site-page"
#toc: false
draft: false
ghcommentid: 0
---

Il y a 2 ans je me suis dit :

> Au prochain article, j'essaye un autre système de blog !

Et depuis 2 ans, plus rien... Je n'ai pas parcouru le web à la recherche de la solution idéale, loin de là, je n'avais juste pas d'idée d'article.

Il y a 3 mois, en regardant une classe de matrice en C++, une idée m'est venue. J'ai écrit mon article puis cherché un système de site statique.

Au départ, j'avais en tête [Octopress][octopress] utilisé par Luc Hermitte pour son [blog][lmghs_blog]. Le principe est d'écrire des fichiers en markdown, de générer le blog et mettre le tout sur github pour avoir une adresse en `github.io`. Le mettre sur github n'est pas une obligation, mais c'est pour moi le plus simple.

J'ai donc essayé [Hugo][hugo]... Ma logique est infaillible :).

## Pourquoi quitter Blogspot ?

Avant de commencer, une petite explication sur pourquoi Blogspot ne m'est pas pratique.

La principale raison est que l'utilisation de Blogspot m'oblige à faire du post-traitement sur mes articles. Pour avoir la couleur dans les codes, ils sont écrits dans un éditeur puis convertits en HTML (via Kate dans mon cas). La couleur de fond est trop contrastée pour le blog ce qui oblige un post-traitement supplémentaire. L'ensemble est plutôt rapide à faire, mais modifier un code est pénible.

Puisque je suis dans le mode HTML de blogspot, je me tape aussi tout le balisage des liens, paragraphes, mots importants, etc, ce qui parasite le texte. L'absence de couleur dans la zone d'édition n'aide pas beaucoup.

Autre point, j'écris rarement un article depuis l'interface web. Déjà parce qu'il faut y accéder -- il y a un peu trop de clics à faire et Blogspot reste lent à charger -- ensuite parce que l'éditeur est trop pauvre. Ça paraît con, mais les possibilités de mon éditeur de code sont très sollicités, même pour l'écriture d'un article. J'envisage même de faire des plugins pour de la saisie rapide.

Ceci fait qu'utiliser un générateur de type markdown me trottait dans la tête depuis un bon moment. Il existe plein de générateurs vers HTML, [Pandoc][pandoc] étant probablement le plus connu. C'est suffisant. On écrit un article, on convertit et on colle la sortie dans la zone d'édition du blog.

J'aurais très bien pu utiliser Blogspot + un générateur. Sauf que je trouve Blogspot lourd à charger et les possibilités de menu et de mise en page sont un peu limitées. Alors autant voir ailleurs !

## Essai de Hugo

En fait, au moment où je commençais à regarder les systèmes existants, quelqu'un sur le [MM de 42][mm42] mit un lien vers Hugo. Le principe reste le même -- au moins pour la génération de blog -- avec comme objectif principal de ne pas avoir de dépendance et de générer rapidement le site.

Un truc qui me plaît bien est de pouvoir utiliser autre chose que le markdown par défaut, par exemple, [Asciidoctor][asciidoc] ou [ReStructuredText][reST]. Les implémentations ne sont pas nativement incorporées à Hugo ce qui rend leur utilisation plus lente dans la génération (il faut appeler un programme externe). Toutefois, il y a en native 2 implémentaitons de [markdown][hugo_content_format] et le [mode Org][org] de Emacs. Pour se faire une idée du mode org, il y a 3 traductions françaises dont 2 à venir de [Pragmatic Emacs](pragmatic_emacs) sur [linuxfr](linuxfr_org).

J'ai essayé Asciidoc, mais il ne fonctionne pas tel quel, il m'a fallu un intermédiaire qui supprime l'option `--safe` pour la coloraion du code (celle-ci utilise [Pygmentize](pygmentize)). Par contre, je trouve le code HTML généré vraiment trop verbeux. À mon sens, entourer les paragraphes de `<div class="paragraph">...</div>` est plus que superflux. Je ne sais pas ce qu'il en est des autres formats ne les ayant pas essayés, pour le moment mes besoins suffisent pour le markdown. À savoir que les formats non natifs ne peuvent pas générer le sommaire, il faut faire un post-traitement sur la sortie HTML.

Une autre chose sympathique que je n'ai pas eu l'occasion d'utiliser concerne les templates Hugo au sein même des documents. Par exemple, un template `image` qui sort un code HTML avec caption + figure + image de la bonne taille + lien pour voir l'image d'origine. En gros, générer un truc bien casse pied à écrire :).

## Et Octopress ?

Bon Hugo c'est bon pour moi, mais j'ai rapidement fait un tour sur Octopress entre deux. Pour tout dire, j'ai rapidement abandonné. Le plugin le plus intéressant pour moi est [Codeblock][octopress_codeblock] et il ne fonctionne pas comme indiqué. J'ai regardé les sources et finalement renoncé.

Au passage, je remarque que le projet n'est plus maintenu depuis plusieurs années et qu'il se base sur [Jekyll](jekyll). J'essaye ce dernier et me retrouve avec des problèmes de dépendances et des packageurs -- il y en a pas qu'un -- qui demandent les droits root pour l'installation. Las, j'abandonne. Finalement, Hugo c'est bien.

## Pour finir

J'ai beaucoup touché au template de Hugo, aussi bien la partie CSS que HTML en me basant sur un des thèmes disponibles. À la base je voulais un thème sombre, mais j'ai finalement adopté un thème clair pour le contenu. Premièrement parce que le thème choisi l'est de base, deuxièmement parce que je trouve le résultat peu satisfaisant. Toutefois, le thème sombre est disponible en le sélectionnant dans le menu du navigateur: Vue -> Style de page -> Night theme (sur firefox, cela varie peut-être sur d'autres navigateurs). À terme, je pense chavirer du côté obscur.

La plupart des morceaux modifiés concernent des éritants que je rencontre sur certains sites et logiciels. J'ai essayé de faire quelque chose d'accessible et de pratique (là je vous vends un objet rare et de grande valeur :D). Du coup, le prochain article y sera consacré. Ça va me changer du c++ et de la méta-programmation.

Et parce que les habitudes ont la vie dure, l'article qui suivra sera dédié au dispatcheur d'un std::variant. La méta-prog, on ne la quitte que les pieds devant :o).

En même temps, les articles de l'ancien blog seront déportés et mis à jour si besoin.


[octopress]:http://octopress.org/
[hugo]:https://github.com/gohugoio/hugo
[pandoc]:http://pandoc.org/
[lmghs_blog]:http://luchermitte.github.io/
[mm42]:https://openclassrooms.com/forum/sujet/groupe-communautaire-groupe-42
[reST]:http://docutils.sourceforge.net/rst.html
[asciidoc]:http://asciidoc.org/
[org]:http://orgmode.org/
[hugo_content_format]:https://gohugo.io/content-management/formats/
[linuxfr_org]:https://linuxfr.org/tags/orgmode/public
[pragmatic_emacs]:http://pragmaticemacs.com/
[pygmentize]:http://pygments.org/
[octopress_codeblock]:http://octopress.org/docs/plugins/codeblock/
[jekyll]:https://jekyllrb.com/
