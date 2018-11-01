---
title: "Paramètres de fonction nommés en C++"
slug: "parametres-de-fonction-nommes-en-cpp"
#description: ""
date: 2015-07-28T00:43:22+01:00
#lastmod: 2018-02-13T03:41:22+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2028
expire: 2048
---

Cet article est la démonstration de l'article précédent. La problématique présentée est la suivante: "Comment, dans une fonction avec plusieurs paramètres optionnels, initialiser un paramètre précis sans indiquer les valeurs optionnelles qui précèdent ?"

La fonction de référence sera la suivante:

```cpp
void draw_rect(
  unsigned w, unsigned h
, char border_top = '-', char border_bottom = '-'
, char border_left = '<', char border_right = '>'
, char fill = '#'
) {
  std::cout << std::setfill(border_top) << std::setw(w+2) << "" << "\n";
  while (h--) {
    std::cout << border_left << std::setfill(fill) << std::setw(w) << "" << border_right << "\n";
  }
  std::cout << std::setfill(border_bottom) << std::setw(w+2) << "" << "\n";
}
```

Comment faire un appel proche de {{<hi cpp "draw_rect(4,3, fill='@')"/>}} ?

## Création d'un paramètre nommé

La première étape consiste à créer un type par paramètre optionnel. Comme je n'ai pas envie de me compliquer la vie, la syntaxe {{<hi cpp "fill='@'"/>}} qui demande plus de code à cause d'une surcharge de {{<hi cpp "operator="/>}} sera remplacée par un simple appel de constructeur {{<hi cpp "fill{'@'}"/>}}.

La définition des types devient alors véritablement simpliste:

```cpp
struct border_top { char value; };
struct border_bottom { char value; };
struct border_left { char value; };

struct border_right { char value; };
struct fill { char value; };
```

## Adapter draw_rect

Au lieu d'adapter `draw_rect`, je vais passer par une surcharge, ceci n'impactera pas le résultat.

La nouvelle fonction doit pouvoir prendre les nouveaux types, mais pas forcément tous et de préférence dans un ordre indéfini.

On pourrait faire toutes les surcharges possibles, il n'y a "que" plus d'une centaine de possibilités après tout... Solution rejetée, évidemment ;).

Une template variadique fera l'affaire.

```cpp
template<class... Ts>
void draw_rect(unsigned w, unsigned h, Ts... params);
```

Il reste maintenant à associer chaque type de `params` avec le paramètre de notre premier prototype de draw_rect.

## Distribution des paramètres

C'est là qu'intervient le magasin de type de l'article précédant (toujours pas trouvé de meilleur nom).

Le principe est simple, toutes les valeurs de params sont regroupées sous une même enseigne appelé ici "pack". On vérifie si le pack est convertible en un type voulu et dans le cas contraire, on utilise une valeur par défaut.

Notre pack ressemble à ça:

```cpp
struct Pack : Ts... {
  Pack(Ts... args) : Ts(args)... {}
} pack{params...};
```

Et la distribution des paramètres se fait ainsi:

```cpp
draw_rect(w, h
, getval<border_top>(pack, '-')
, getval<border_bottom>(pack, '-')
, getval<border_left>(pack, '<')
, getval<border_right>(pack, '>')
, getval<fill>(pack, '#')
);
```

Pour les plus attentifs (il m'a fallu 2 jours pour le réaliser…), rien n'empêche d'envoyer des paramètres inutiles. On peut l'empêcher grâce à un {{<hi cpp "static_assert"/>}} avec une condition qui ressemble à

```cpp
sizeof...(Ts) == std::is_convertible_t<border_top>()
               + std::is_convertible<border_bottom>()
               + /*etc*/
```

Dans l'histoire, bien que largement surmontable, getval est la fonction la plus compliquée. Si Pack est convertible en `T` alors `Get::get()` est utilisé, sinon `Default::get()`.

```cpp
template<class T, class Pack>
char getval(Pack& pack, char default_) {
  struct Get     { static char get(T item, char        ) { return item.value; } };
  struct Default { static char get(Pack&, char default_) { return default_;   } };
  return std::conditional<std::is_convertible<Pack, T>::value, Get, Default>::type
  ::get(pack, default_);
}
```

## Aller plus loin

Les choses se compliquent quand on veut récupérer un type partiel. Un {{<hi cpp "basic_fill<T>"/>}} par exemple. Actuellement, rien n'existe dans le standard et il faudra jouer avec les paramètres template template et un [appel conditionnel de fonction selon la validité d'une expression]({{<ref "appel-conditionnel-de-fonction-selon-la-validite-d-une-expression.md">}}).

Pour compléter la solution, il faudrait aussi prendre en compte les références et ajouter des contraintes sur le paramètre.

Pour continuer dans la voie des paramètres nommés, il existe `Boost.Parameters` et des variantes plus modernes telles que [parameter2](https://github.com/odinthenerd/parameter2).
