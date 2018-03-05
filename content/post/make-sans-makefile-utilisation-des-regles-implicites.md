---
title: "make sans Makefile, utilisation des règles implicites"
#description: ""
date: 2013-06-26T11:05:28+01:00
#lastmod: 2018-02-13T03:36:19+01:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "make" ]
draft: false
ghcommentid: 0
---

La commande GNU make possède énormément de commandes implicites regroupés dans un Makefile "par défaut" avec plein de règles. Celui-ci est visible en tapant `make -pf /dev/null` dans un terminal.

Rien qu'avec ça, on peux compiler des fichiers C, C++, archive, latex, etc. Il y a de quoi faire en fait.

Par exemple, je crée un fichier C nommé `test.c`:

```c
int main(int ac, char** av)
{
  return ac;
}
```

Que je compile avec `make test` pour crée l'exécutable `test`.
Mais si on veut avoir des fichiers objets, il faut le demander explicitement: `make test.o test`.

Maintenant, on remplace `test.c` par `test.cpp` et on inclut {{<hi cpp "<iostream>"/>}}.

La compilation se fait encore avec `make test` ou `make test.o test` pour avoir les fichiers objets.

Ah ! En fait non, le second ne fonctionne pas.
`test.o` est compilé avec `cc`, et comme {{<hi cpp "<iostream>"/>}} est linké avec une lib, l'implémentation du constructeur [std::ios_base::Init](http://en.cppreference.com/w/cpp/io/ios_base/Init) n'est pas trouvée. L'idéal serait d'utiliser `g++` ou un autre compilateur C++ pour avoir les chemins de bibliothèque convenablement configurées.

Si ce n'est qu'une question de compilateur, changeons-le !

Tout d'abord, la règles de compilation d'un `.o` vers un exécutable:

```sh
make -pf/dev/null | grep '%: %.o' -A3
```

```Makefile
%: %.o
#  commands to execute (built-in):
        $(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
```

Cela utilise pour compiler la variable `LINK.o`. Soit, qu'est sa valeur ?

```sh
make -pf/dev/null | g 'LINK.o ='
```

```Makefile
LINK.o = $(CC) $(LDFLAGS) $(TARGET_ARCH)
```

Qui fait référence à `CC`. On s'approche, vérifions:

```sh
make -pf/dev/null | g 'CC ='
```

```Makefile
CC = cc
```

Bingo \o/. Il suffit donc de changer la valeur de `CC`.

On recommence avec `make CC=g++ test.o test` et ça fonctionne :).

Cette petite excursion permet mine de rien de découvrir pas mal de variables. Au passage, il est plus intéressant de les utiliser que d'en recréer des nouvelles. Idem dans les règles de dépendances. S'il faut les personnaliser, autant garder les mêmes commandes à exécuter.

Par exemple, pour compiler un `.cpp` en `.o`:

```Makefile
%.o: %.cpp
#  commands to execute (built-in):
        $(COMPILE.cpp) $(OUTPUT_OPTION) $&lt;
```

Généralement les sources sont également dépendantes de fichier d'en-tête. Si on fait une règle qui prend en compte cela, autant garder la même commande:
(petite parenthèse pour dire que gcc possède l'option `-MM` pour lister les dépendances entre les fichiers.)</p>

```Makefile
bidule.o: bidule.cpp bidule.h machin.h
        $(COMPILE.cpp) $(OUTPUT_OPTION) $&lt;
```

Ainsi, quand `CXXFLAGS` sera modifié, la règle le prendra en compte car

```Makefile
COMPILE.cpp = COMPILE.cc
```

Et

```Makefile
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
```

À noter que la variable `CPPFLAGS` fait référence à la commande `cpp` (ou option `-E` de gcc) et concerne le [C-preprocessor](http://fr.wikipedia.org/wiki/Pr%C3%A9processeur_C).

Bien sûr, la technique du Makefile seul a des limites. Par exemple, s'il y a plusieurs fichiers C, je peux créer tous les fichiers objets avec `make *.o`, mais l'exécutable ne sera dépendant que d'un fichier. Au final les règles par défaut ne permettront pas de créer l'exécutable, il faudra faire `gcc *.o` ou un Makefile avec au minimum `a.out: *.o`.

Voilà, ce fut un petit post pour découvrir l'existence des règles implicites et des variables prédéfinies :).

[Manual de make](http://www.gnu.org/software/make/manual/make.html)

make -p <(echo 'a.out: *.o')
