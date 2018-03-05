---
title: "Valeur, référence ou pointeur ? (1/2)"
#description: ""
date: 2014-05-22T09:51:53+01:00
#lastmod: 2018-02-13T03:23:53+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

> Quand utiliser une variable par valeur, référence ou pointeur ?

Telle fut la question qui m'a été posée :p.

Comme je ne suis pas entièrement satisfait de la réponse que j'ai donné, je fais un article. Pour tout dire, la réponse n'est pas aussi triviale que l'on pourrait le croire depuis l'arrivée du C++11 et la sémantique de déplacement.

Tout d'abord, décomposons cette question en 2 parties:

- Valeur ou référence constante ?
- Référence ou pointeur ?

Je réponds ici à la première, la seconde fera l'objet d'un autre article.


## Valeur ou référence constante ?

Le choix se justifie en majorité par un besoin d'optimisation. Une prise par valeur induit forcément une copie, cette dernière pouvant être extrêmement coûteuse. Par exemple, la copie d'un `std::vector` ne se fait pas en un claquement de doigt, il y a tout un attirail derrière: allouer un espace mémoire et copier tous les éléments du vecteur précédent qui peuvent eux-mêmes faire des opérations complexes.

À contrario, la référence constante est un alias vers une variable. Il n'y a jamais de copie.

J'insiste bien sur référence **constante** car, pour être au plus proche de l'effet d'une copie, l'objet d'origine ne doit pas bouger. De plus, une instance constante ne peut appeler que des fonction membre constantes, ce qui assure une invariance (cf: const-correctness).

Donc, référence constante pour les valeurs qui ne sont pas modifiés dans la fonction. Une règle dit: "tout ce qui est plus grand qu'un pointeur pourrait être passé par référence constante". Je préfère dire tous les types en référence constante sauf les [fondamentaux](http://en.cppreference.com/w/cpp/language/types) (int, float, etc). Même si mettre une référence constante sur un `int` n'est pas une erreur, je n'adhère pas vraiment.

Une seule exception cependant, quand le paramètre va de toute façon être copié localement dans la fonction pour être modifié. On pourrait croire que le résultat sera le même, mais c'est être naïf.

```cpp
class BigInt { … };
operator+(const BigInt & a, const BigInt & b)
{
  BigInt ret(a);
  ret += b;
  return ret;
}
```

Supposons que BigInt fasse de l'allocation dynamique pour représenter les nombres. Avec ce code:

```cpp
BigInt n1(1);
BigInt n2 = BigInt(2) + n1;
```

Il y a 3 allocations:

- n1
- BigInt(2)
- ret

Alors que cette implémentation de `operator+` n'en produit que 2.

```cpp
operator+(BigInt a, const BigInt & b)
{
  a += b;
  return a;
}
```

Car il y a [copy elision](http://en.cppreference.com/w/cpp/language/copy_elision) (c'est le même principe que la RVO mais pour les paramètres). (Voir aussi [ici et la réponse de Flob90](http://cpp.developpez.com/actu/53711/Operateur-d-affectation-copie-implicite-ou-explicite/).


## Mais ça c'était avant...

Maintenant qu'il y a la [sémantique de déplacement](http://h-deb.clg.qc.ca/Sujets/Divers--cplusplus/Mouvement.html), les copies sont préférées quand une fonction recevant le paramètre va, quoi qu'il arrive, le copier dans une variable membre. Premièrement parce que l'utilisateur pourra faire un `std::move` de sa variable pour s'en "débarrasser" car il n'en a plus besoin. Deuxièmement parce que la fonction a besoin d'une copie et le compilateur le fera pour nous.

En comparaison avec une copie sur le `std::vector`, le move-constructor et le move-assignment sont extrêmement rapide: 3 affections de pointeur pour chaque vecteur.

Par exemple avec cette base:

```cpp
#include <utility>
#include <vector>

using vector_int_t = std::vector<int>;

class A {
  vector_int_t c;

public:
  A(vector_int_t cont)
  : c(std::move(cont))
  {}
};
```

Le code suivant fait 2 allocations (comme avec les références constantes)

```cpp
vector_int_t c{1, 2};
A a(c);
```

Alors que celui-ci qu'une seule

```cpp
vector_int_t c{1, 2};
A a(std::move(c));
// c.size() == 0;
```

Et ce dernier aussi

```cpp
A(vector_int_t{1, 2});
// ou A({1 ,2});
```

Mais si le type ne possède pas de constructeur de déplacement, celui de copie sera utilisé et alors une référence constante est probablement mieux.


## Quand la copie se fait sous condition

Il existe des paramètres pouvant être copiés, mais pas toujours. Dans ce cas, bien que la référence constante reste une bonne solution, une version prenant aussi une temporaire (rvalue ici) est probablement mieux. Mais si les types ne sont pas abstraits (comprendre full template) alors il faudra faire 2 versions: une avec rvalue et une avec constref. Ce qui se traduit, quand le code est un peu long, par l'ajout d'une fonction de prédicat ou une version template privée appelé par les 2 autres.

Les `&&` sur les types full templates ont 2 états possibles: rvalue ou lvalue ([catégorie de valeurs](http://en.cppreference.com/w/cpp/language/value_category).

```cpp
struct Foo {
  void foo(std::string const& s) { privfoo(s); }
  void foo(std::string&& s) { privfoo(std::move(s)); }

private:
  template<class String>
  // ici && représente soit rvalue, soit une référence (constante)
  void privfoo(String&& s) {
    // …
    if (xyz) {
      // soit un move-assignment soit un copy-assignment
      str = std::forward<String>(s);
    }
  }

  std::string str;
};
```

Ou

```cpp
struct Bar {
  void bar(std::string const& s) { if (check(s)) str = s; }
  void bar(std::string&& s) { if (check(s)) str = std::move(s); }

private:
  bool check(std::string const& s) {
    // …
    return xyz;
  }

  std::string str;
};
```


## Prise d'objet non-copiable sous condition

Par exemple, donner la propriété d'un [std::unique_ptr](http://en.cppreference.com/w/cpp/memory/unique_ptr) à une classe selon certains prérequis décidés par une fonction. Contrainte supplémentaire, l'objet n'est pas copiable.

- Une référence constante n'est pas envisageable, l'objet ne pouvant pas être déplacé car constant.
- Une prise par valeur non plus (grâce à std::move), car la ressource serait systématiquement transmise à la fonction même si cette dernière ne la garde pas. L'appelant est dans l'incapacité de le savoir et perd la ressource.
- Une référence non-constante est possible, mais il ne sera alors pas possible d'envoyer un temporaire. Il faudra obligatoirement passer par une variable intermédiaire ce qui est désagréable quand on ne va rien en faire.
- Reste la rvalue avec laquelle une temporaire fonctionne, mais il faudra automatiquement faire un std::move quand la variable est une référence. Cela a l'avantage d'informer l'utilisateur sur l'éventuel déplacement de ressource.

Au final, bien qu'une référence fonctionne, seule une rvalue est pratique à l'usage. Seulement, aucunes de ces méthodes n'indiquent une prise partielle, seule la documentation nous le dira. Ceci pourrait par contre être une convention d'écriture: si une ressource non copiable est prise par rvalue, alors la fonction est libre de se l'approprier quand certaines conditions internes sont remplies.

## Quand les opérations ne sont pas connues

Reste la dernière situation: les templates. Les règles sont les mêmes qu'avant mais si le rôle du paramètre n'est pas défini (comprendre la fonction ne fait rien d'autre qu'envoyer le paramètre à une autre fonction ou que le qualifier importe peu) les paramètres sont à prendre par référence universelle (`T&&` pour les template). À ce moment, toutes les utilisations de cette variable devraient se faire par l'intermédiaire de [std::forward](http://en.cppreference.com/w/cpp/utility/forward), même lorsqu'une fonction membre est utilisées (la faute au [qualificateur de référence sur fonction membre](http://en.cppreference.com/w/cpp/language/member_functions#const-.2C_volatile-.2C_and_ref-qualified_member_functions).

Toutefois, attention de ne pas déléguer plusieurs fois la responsabilité et de ne faire std::forward (et std::move) que sur la dernière utilisation de la variable.

Cependant, certains objets de par leur concept seront pris par valeur. Comme les itérateurs pour la bonne raison que leurs états changent dans l'execution de la fonction. On peut toutefois prendre l'itérateur de fin par référence constante s'il n'est pas modifié.


## Pas de référence constante pour les observers

Bien que cela sorte du cadre de la question d'origine, il ne faut pas prendre par référence constante une valeur à observer.

J'entends par observer les variables qui sont gardées en lecture dans le but de vérifier leur état à un instant t.

Les références constantes peuvent être des temporaires à leur construction (Jusque-là c'est défini par la norme: prolongement de la durée de vie d'une temporaire). Le problème vient du déplacement vers un scope parent. La valeur temporaire est détruite, mais la référence est gardée ; référence sur une valeur qui n'existe plus. Cela débouche sur un comportement indéfini et, dans le meilleurs des cas, un segfault.

[Un article qui présente une situation similaire avec une lambda retournant T à travers std::function&lt;const T&>.](http://blog.developpez.com/gpu/?p=309)

```cpp
#include <iostream>
#include <string>

struct Validate
{
  const std::string & s;
  void display() const { std::cout << s; }
};

Validate f()
{ return {"plop"}; }

int main()
{
  Validate x = f();
  x.display();
}
```

(Moi j'ai un segfault)

Pour limiter ce bug, il faut empêcher de prendre une rvalue. Soit avec les constucteurs suivants:

```cpp
Validate(std::string&&)=delete;
Validate(const std::string & s):s(s){}
```

Soit en utilisant [std::reference_wrapper](http://en.cppreference.com/w/cpp/utility/functional/reference_wrapper) (et [std::cref](http://en.cppreference.com/w/cpp/utility/functional/ref)).

Ou, peut-être mieux, faire un objet `observable` tout pareil que `std::reference_wrapper`, mais avec constructeur explicite. L'intérêt d'utiliser l'un des 2 objets cités et de focaliser l'utilisateur sur l'aspect "j'ai besoin que cette variable vive au moins aussi longtemps que moi".


## Résumé

- Valeur pour les types fondamentaux (int, double, etc), ceux modifiés sans que l'utilisateur n'ai besoin de le savoir (ex: itérateurs ou premier paramètre de l'opérateur '+') ou les foncteur sans états (prédicats, comparateurs, ...).
- Valeur + std::move quand l'objet peut être déplacé (possède un move-ctor ou/et move-assign non trivial (ex: std::string, std::vector, ...).
- Valeur pour les ressources non copiables à transférer (std::unique_ptr, ...).
- Référence constante pour les paramètres en lecture seule ou ne disposant pas de move-ctor ou/et move-assign non trivial.
- Référence constante et rvalue quand le paramètre <em>peut être</em> copié et possède un move-ctor/move-assign non trivial.
- Rvalue pour les ressources non copiables avec déplacement conditionnel (std::unique_ptr, ...).
- Référence constante pour les types inconnus (template) qui n'ont pas d'intérêt à être pris par valeur (ou au pire, stratégie variable selon le résultat de std::is_trivially_*/std::is_copy_*/std::is_move_*).
- Rvalue pour les types inconnus (template) quand le paramètre n'a pas de rôle direct dans la fonction ou que le qualificateur n'importe pas (ne pas oublier std::forward pour le transmettre à une autre fonction (seulement s'il n'est plus utilisé ensuite)).

[Partie 2]({{<ref "valeur-reference-ou-pointeur-(2-2).md">}})
