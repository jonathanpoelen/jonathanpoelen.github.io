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
draft: true
ghcommentid: 0
---

Il y a 2 ans je me suis dit :

> Au prochain article, j'essaye un autre système de blog !

Et depuis 2 ans, plus rien... Je n'ai pas parcouru le web à la recherche de la solution idéale, loin de là, je n'avais juste pas d'idée d'article.

Il y a quelques semaines, en regardant une classe de matrice en C++, une idée m'est venu. J'ai écrit mon article puis cherché un système de site statique.

Au départ, j'avais en tête [Octopress][octopress] utilisé par Luc Hermitte pour sont [blog][lmghs_blog]. Le principe est d'écrire des fichiers en markdown, de générer le blog et mettre le tout sur github pour avoir une adresse en `github.io`. Le mettre sur github n'est pas une obligation, mais c'est pour moi le plus simple.

J'ai donc essayé [Hugo][hugo]... Ma logique est infaillible :).

## Essai de Hugo.

En fait, au moment où je commencais à regarder les systèmes existant, quelqu'un sur le [MM de 42][mm42] mit un lien vers Hugo. Le principe reste le même -- au moins pour la génération de blog -- avec comme objectif principal de ne pas avoir de dépendance et de générer rapidement le site.

Un truc qui me plait bien est de pouvoir utiliser autre chose que le markdown par défaut, par exemple, [Asciidoctor][asciidoc] ou [ReStructuredText][reST]. Les implémentations ne sont pas nativement incorporées à Hugo ce qui rend leur utilisation plus lente dans la génération (il faut appeler un programme externe). Toutefois, il y a en native 2 implémentaitons de [markdonw][hugo_content_format] et le [mode Org][org] de Emacs. Pour se faire une idée du mode org, il y a 3 traductions française dont 2 à venir de [Pragmatic Emacs](pragmatic_emacs) sur [linuxfr](linuxfr_org).

J'ai essayé Asciidoc, mais il ne fonctionne pas tel quel, il m'a fallu un intermédiaire qui vire l'option `--safe`. Par contre, je trouve le code html généré vraiment trop verbeux. À mon sens, entourer les paragraphes de `<div class="paragraph">...</div>` est plus que superflux. Je ne sais pas ce qu'il en est des autres formats ne les ayant pas essayés, pour le moment mes besoins suffisent pour le markdown. À savoir que les formats non natifs ne peuvent pas génèrer le sommaire, il faut faire un post-traitement sur la sortie html.

Une autre chose sympatique que je n'ai pas eu l'occasion d'utiliser concerne les templates Hugo au sein même des documents. Par exemple, un template `image` qui sort un code html avec caption + figure + image de la bonne taille + lien pour voir l'image d'origine. En gros, générer un truc bien casse pied à écrire :).

## Et Octopress ?

Bon Hugo c'est bon pour moi, mais j'ai rapidement fait un tour sur Octopress entre-deux. Pour tout dire, j'ai rapidement abandonné. Le plugin le plus interessant pour moi est [Codeblock][octopress_codeblock] et il ne fonctionne pas comme indiqué. J'ai regardé les sources et finalement renoncé. Au passage, je remarque que le projet n'est plus maintenu depuis plusieurs années et qu'il se base sur [Jekyll](Jekyll). J'essaye ce dernier, me retrouve avec des problèmes de dépendances, installe divers choses et peste sur le fait qu'il faille être root. Configure le packageur de ruby avec bien du mal pour me retrouver avec des installations silencieuses qui utilisent un autre installateur qui demande lui aussi les droits root ! Las, j'abandonne. Finalement, Hugo c'est bien.

## Pour finir

J'ai beaucoup touché au template de Hugo, aussi bien la partie CSS que HTML en me basant sur un des thèmes disponible. À la base je voulais un thème sombre, mais j'ai finalement adopté un thème clair pour le contenu. Premièrement parce que le thème choisi l'est de base, deuxièmement parce que je trouve le résultat peu satisfaisant. Toutefois, le thème sombre est disponible en le sélectionnant dans le menu du navigateur: Vue -> Style de page -> Night theme (sur firefox, cela vari peut-être sur d'autre navigateur). À terme, je pense chavirer du côté obsure.

La plupart des morceaux modifiés concernent des éritants que je rencontre sur certains sites et logiciels. J'ai éssayé de faire quelque chose d'accessible et de pratique (là je vous vends un objet rare et de grande valeur :D). Du coup, le prochain article y sera consacré. Ça va me changer du c++ et de la méta-programation.

Et parce que les habitudes ont la vie dure, l'article qui suivra sera dédié au dispatcheur d'un std::variant. La méta-prog, on ne la quitte que les pieds devant :o).

En même temps, les articles de l'ancien blog seront déportés et mis à jour si besoin.



[lmghs_blog]:http://luchermitte.github.io/
[mm42]:https://openclassrooms.com/forum/sujet/groupe-communautaire-groupe-42
[reST]:http://docutils.sourceforge.net/rst.html
[asciidoc]:http://asciidoc.org/
[org]:http://orgmode.org/
[hugo_content_format]:https://gohugo.io/content-management/formats/
[linuxfr_org]:https://linuxfr.org/tags/orgmode/public
[pragmatic_emacs]:http://pragmaticemacs.com/
[octopress_codeblock]:http://octopress.org/docs/plugins/codeblock/
[jekyll]:https://jekyllrb.com/
