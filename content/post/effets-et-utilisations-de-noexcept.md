---
title: "Effets et utilisations de noexcept"
#thumbnail: ""
#description: ""
date: 2019-09-01T16:09:32+02:00
#lastmod: 2019-08-18T18:09:32+02:00
slug: "effets-et-utilisations-de-noexcept"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2028
---

## Fonction noexcept

`noexcept` est un mot clef apparu en C++11. Dans le prototype d'une fonction, il indique que cette dernière ne jette pas d'exception. Cela ne veut pas dire qu'aucune exception ne sera présente dans la fonction, mais qu'**aucune exception ne sortira** de la fonction. Dans le cas contraire, le programme s'arrête subitement avec un appel à [`std::terminate`](https://en.cppreference.com/w/cpp/error/terminate).

`noexcept` n'impose aucune restriction sur ce que peut faire la fonction. Il est tout à fait possible d'utiliser des fonctions qui ne sont pas marquées `noexcept` à l'intérieur d'une fonction `noexcept`, voire, de jeter des exceptions. La seule contrainte se trouve sur le chemin de sortie: il ne doit pas se faire avec une exception.

Voici un code totalement inutile de démonstration avec une exception qui traverse une fonction `foo()` marquée `noexcept`:

```cpp
void foo() noexcept
{
  throw 42; // noexcept ? Pas vu
}

#include <iostream>

int main()
{
  try {
    foo(); // le programme s'arrête ici
  }
  catch (...) {
  }
  std::cout << "ok\n"; // ne s'affiche jamais
}
```

La compilation puis l'exécution donne:

```cpp
$ g++ test.cpp && ./a.out
test.cpp: In function ‘void foo()’:
test.cpp:3:9: warning: throw will always call terminate() [-Wterminate]
    3 |   throw 42;
      |         ^~
terminate called after throwing an instance of 'int'
zsh: abort (core dumped)  ./a.out
```

G++, Clang et MSVC émettent tous 3 un avertissement et le programme explose comme attendu.

L'avertissement ici est évident, mais ce n'est pas toujours le cas, le compilateur ne pouvant garantir que toutes les fonctions utilisées dans une fonction noexcept ne jettent pas elles-mêmes des exceptions.

Notre fonction `foo()` est une énorme bombe à retardement. Chouette, une raison supplémentaire pour faire planter un programme ;)


## Les bénéfices de noexcept

Une mauvaise utilisation de `noexcept` peut faire planter un programme. Mais une fonctionnalité à risque ne vient jamais sans avantage. Puisque `noexcept` garantit qu'aucune exception ne sorte de la fonction, le compilateur peut éjecter le code qui s'en occupe. Si les fonctions ne lancent pas d'exception, il peut déterminer avec certitude quels sont les chemins de sortie, supprimer du code, réordonner les instructions et appliquer autres obscures magies dont il a le secret.

Dit comme ça, on pourrait croire que plein d'optimisations s'offrent à nous, mais non. Premièrement, les compilateurs sont capables de faire des exceptions qui n'ont de coût qu'au moment de l'appel. Bien sûr, l'exécutable est plus gros et peut impacter le cache d'instruction, mais le coût d'une exception non utilisée est nulle. Secundo, ce n'est pas évident de produire un exécutable qui supprime vraiment du code, à moins d'avoir absolument toutes les fonctions en `noexcept`, la magie du compilateur se verra limité. Surtout que les bonnes options d'optimisation donnent des résultats comparables.

Après plusieurs essais d'exemples de code pseudo-réaliste, en voici 2 basés sur une fonction très simple

```cpp
// foo.cpp
int foo(int x) NOEXCEPT
{
  return x;
}
```

`NOEXCEPT` sera à remplacer par `noexcept` ou rien. Respectivement les options de compilation `-DNOEXCEPT=noexcept` et `-DNOEXCEPT=`.

Ce premier exemple prouve que le compilateur est capable de prendre en compte `noexcept` pour supprimer du code.

```cpp
// main.cpp
#include <iostream>

int foo(int) NOEXCEPT;

void bar(std::ostream& out)
{
  char const* err;
  try {
    // trace de l'étape en cours
    err = "foo step";
    auto r = foo(1);
    // fin du "traitement"
    err = nullptr;

    out << r; // ceci peut lancer une exception std::ios_base::failure
  }
  catch(std::exception const& e)
  {
    // err != nullptr uniquement si foo() lance une exception
    if (err)
      std::cerr << "oups: " << err << "\n";
    std::cerr << e.what() << "\n";
  }
}
```

Ce code permet d'afficher un message sur l'étape en cours avant une éventuelle exception. Dans notre cas, `foo()` ne lance pas d'exception, mais en l'absence de `noexcept`, `main.cpp` ne le sait pas.

Si `foo()` ne lance pas d'exception, alors `err` sera toujours égal à `nullptr` dans le `catch`. Il est alors possible de supprimer la condition qui serait toujours évaluée à `false`, ce qui doit donner un exécutable plus petit.

Le petit script suivant va compiler chaque `.cpp` avec et sans `noexcept` puis afficher la taille de l'exécutable.

```bash
for x in noexcept '' ; do
  d=NOEXCEPT=$x
  g++ -O3 -D$d *.cpp &&
  echo $d &&
  stat ./a.out *.o --format="$(echo "%n\t%s")"
done
```

&nbsp;         | NOEXCEPT= | NOEXCEPT=noexcept
---------------|-----------|------------------
`stat ./a.out` | 17632     | 16816


Pronostic vérifié. On peut même regarder l'[assembleur sur godbolt](https://godbolt.org/z/xBmjdI) pour s'en convaincre.

Voici un second exemple plus simple qui montre que le compilateur supprime totalement la gestion des exceptions lorsqu'il lui est permis de le faire.

```cpp
#include <memory>

int foo(int) NOEXCEPT;

int bar(std::unique_ptr<int>&& p)
{
    // some stuff
    // ...
    auto u = std::move(p);
    return foo(*u);
}
```

Et l'asm de https://godbolt.org/z/-yIB7m

```asm
bar(std::unique_ptr<int, std::default_delete<int> >&&):
        push    r12
        push    rbp
        sub     rsp, 8
        mov     rbp, QWORD PTR [rdi]
        mov     QWORD PTR [rdi], 0
        mov     edi, DWORD PTR [rbp+0]
        call    foo(int)
        mov     rdi, rbp
        mov     esi, 4
        mov     r12d, eax
        call    operator delete(void*, unsigned long)
        add     rsp, 8
        mov     eax, r12d
        pop     rbp
        pop     r12
        ret
; ce qui suit ne concerne que l'éventuelle exception de foo()
        mov     r12, rax
        jmp     .L2
bar(std::unique_ptr<int, std::default_delete<int> >&&) [clone .cold]:
.L2:
        mov     rdi, rbp
        mov     esi, 4
        call    operator delete(void*, unsigned long)
        mov     rdi, r12
        call    _Unwind_Resume
```

La seconde partie du code asm n'est utilisée que pour le traitement d'une exception, elle se situe en dehors du flux normal d'exécution et disparait lorsque foo() ne lance pas d'exception.

Maintenant, il faut savoir qu'avec l'option `-flto` la taille des exécutables sont les mêmes que foo() soit `noexcept` ou pas. Simplement parce que le compilateur déduit que foo() ne lance pas d'exception. Mais dans le cas de bibliothèque partagée, `-flto` ne pourra rien faire et la différence subsistera.

Puisque le compilateur peut déduire lui-même qu'une fonction ne lance pas d'exception, pourquoi et surtout quand mettre noexcept ?


## Quand mettre noexcept

Déjà, on peut dire qu'une fonction qui ne lance absolument pas d'exception peut être `noexcept`. Cette information est surtout utile dans les APIs ou bibliothèques qui ont cette garantie. Un analyseur statique pourrait aussi vérifier qu'aucune exception ne la traverse. Dans le cas contraire, c'est plutôt risqué.

Néanmoins, `noexcept` est vraiment important sur les constructeurs déplacement.

Pour être plus précis, conjointement avec les conteneurs de la STL, cela va déterminer si le conteneur utilise ou non le déplacement. Si la fonction est `noexcept`, le déplacement d'un élément ne peut pas échouer. Dans le cas d'un `std::vector<T>`, cela veut dire qu'après un agrandissement de la capacité, tous les éléments peuvent être déplacés sans échec. Par contre, si le déplacement lance une exception, une partie des données pourrait se perdre et `std::vector` utilise alors la copie. On parle de résistance aux exceptions ou d'[exception-safety](https://en.wikipedia.org/wiki/Exception_guarantees). Dans le cas de `std::vector`, c'est une garantie forte: l'état du vector est inchangé s'il y a une exception sur l'augmentation de la capacité. Pour en savoir un peu plus sur l'exception-safety, c'est [par là](https://www.boost.org/community/exception_safety.html).

Pour illustrer l'influence de `noexcept` avec std::vector, voici une petite classe qui affiche quel constructeur est utilisé. Le constructeur de déplacement est toujours présent, seule la présence de `noexcept` diffère.

```cpp
#include <iostream>

struct A
{
  A(int i) : i(i) {}

  A(A const& a) : i(a.i)
  { std::cout << "A&(" << i << ")\n"; }

  A(A&& a) NOEXCEPT : i(a.i)
  { std::cout << "A&&(" << i << ")\n"; }

  int i;
};

#include <vector>

int main()
{
  std::vector<A> v;
  v.emplace_back(1);
  v.emplace_back(2); // agrandissement de la capacité + déplacement/copie de A(1)
}
```

Et les résultats après compilation:

&nbsp;  | NOEXCEPT=               | NOEXCEPT=noexcept
--------|-------------------------|------------------
./a.out | {{<hi cpp "A&(1)"/>}}   | {{<hi cpp "A&&(1)"/>}}

Maintenant imaginons que notre classe `A` contienne des `std::string` et des `std::vector`. Si le constructeur de déplacement n'est pas `noexcept`, alors il y aura de nombreuses copies qui auront un gros impact sur les performances. Ce n'est généralement pas ce qu'on veut.

Personnellement, je déconseille d'écrire le constructeur et operator= de déplacement ou de copie. 99% du temps, la version par défaut fait le job, le compilateur déduisant lui-même `noexcept` pour les fonctions par défaut -- et c'est bien le seul moment. S'il y a une véritable nécessité, il est généralement possible d'avoir une classe qui ne fournit que ce service et l'utiliser en variable membre.

Si on écrit explicitement `noexcept` sur une fonction par défaut -- par exemple {{<hi cpp "X(X&&) noexcept = default;"/>}} -- le compilateur va **vérifier** que le déplacement de chaque membre ne lance pas d'exception. Et dans le cas contraire, ne compile pas.

Pour finir, **les destructeurs sont implicitement `noexcept`**. Si un destructeur balance une exception, le programme va s'arrêter. Les raisons sont assez simples: il est difficile de catcher une exception d'un destructeur et il est impossible d'arriver à un état cohérent sans code spécifique si les objets ne peuvent être détruits. De plus, il faut savoir que jeter une exception pendant le traitement d'une exception appelle automatiquement `std::terminate()`. Du coup, les exceptions dans un destructeur sont plutôt une mauvaise idée.

Mais si on veut autoriser le destructeur à jeter des exceptions ou pouvoir marquer nos fonctions en `noexcept` à la seule condition qu'une expression précise soit elle-même `noexcept` il existe un nouveau mot clef: `noexcept`. Oui, mais non, il est différent.


## Spécificateur d'exception et opérateur noexcept

Il existe 2 mots clef pour `noexcept`: le spécificateur d'exception et l'opérateur.

- Le spécificateur `noexcept` ou `noexcept(bool)` se met toujours dans le prototype de la fonction. `noexcept` et `noexcept(true)` indiquent que la fonction ne jette pas d'exception, `noexcept(false)` indique que la fonction jette potentiellement une exception.
- L'opérateur `noexcept(expr)` prend une expression est retourne un booléen qui indique si oui ou non une expression peut lancer une exception.

Voici quelques exemples:

```cpp
constexpr bool foo() { return true; }

void f1() noexcept;
void f2() noexcept(false);
void f3() noexcept(foo()); // -> noexcept(true)
void f4() noexcept(noexcept(f1())); // noexcept seulement si f1() est noexcept
void f5() noexcept(noexcept(new int));
void f6() noexcept(noexcept(false));

std::cout << noexcept(f1())  << "\n"; // true
std::cout << noexcept(f2())  << "\n"; // false
std::cout << noexcept(f3())  << "\n"; // true
std::cout << noexcept(f4())  << "\n"; // true
std::cout << noexcept(f5())  << "\n"; // false
std::cout << noexcept(f6())  << "\n"; // true
std::cout << noexcept(true)  << "\n"; // true
std::cout << noexcept(false) << "\n"; // true
```

Pas très compliqué dans l'ensemble, même si bien que logique, le résultat de `f6()` et la dernière ligne peuvent surprendre: `false` est une expression qui ne retourne jamais d'exception, par conséquent, l'opérateur `noexcept` retourne `true`.

La STL offre aussi les traits [`std::is_nothrow_*`](https://en.cppreference.com/w/cpp/types#Supported_operations) pour vérifier si certaines expressions sont `noexcept` ; ainsi qu'une fonction [`std::move_if_noexcept`](https://en.cppreference.com/w/cpp/utility/move_if_noexcept) qui retourne une rvalue si le constructeur de déplacement est noexcept et une lvalue constante dans le cas inverse.

Avant C++11, il existait le spécificateur `throw()` pour indiquer qu'une fonction ne jette pas d'exception. Son comportement est très différent et pas du tout efficace. En C++17, il devient l'équivalent de `noexcept(true)` pour finalement être supprimé en C++20.


## Signature et type de fonction

2 fonctions qui ne diffèrent que par la spécification d'exception ne peuvent être surchargées car leur signature est identique. Ceci n'est pas autorisé:

```cpp
void foo();
void foo() noexcept; // erreur
```

Du fait de la signature identique, `noexcept` peut très bien apparaître dans le prototype d'une fonction exposée dans une bibliothèque C ({{<hi cpp "extern \"C\""/>}}), mais cette information disparaît dans la bibliothèque elle-même. Mieux que ça, si la bibliothèque est compilée sans `noexcept`, mais que le `.h` distribué les met, le compilateur retrouve ses petits, car le name mangling est le même.

Par contre, avant C++17 le type d'une fonction ne prend jamais en compte `noexcept`. C’est-à-dire que les types `void(*)()` et `void(*)() noexcept` sont identiques.

À partir de C++17, `noexcept` fait partie intégrante du type de la fonction. Un pointeur de fonction `noexcept` n'est pas compatible avec un pointeur de fonction qui jette potentiellement une exception. Néanmoins, une fonction `noexcept` est convertible en un pointeur de fonction qui n'est pas `noexcept`. Les posts conditions sont plus fortes ce qui ne brise pas le [LSP](https://fr.wikipedia.org/wiki/Principe_de_substitution_de_Liskov).

```cpp
void foo() noexcept;
void bar();

void(*f)() noexcept = foo;
void(*f)() noexcept = bar; // non depuis C++17

void(*g)() = bar;
void(*g)() = foo; // ok
```

Cette caractéristique s'applique aussi aux fonctions `virtual`.

```cpp
class A
{
  virtual void foo() noexcept = 0;
  virtual void bar() = 0;
};

class B : A
{
  void foo() override; // il manque noexcept
  void bar() noexcept override; // ok, plus de restriction sur les conditions de sortie
};

class C : B
{
  void bar() override; // erreur, même si A::bar() n'est pas noexcept, B::bar() l'est
};
```

## Ce qu'il faut retenir

Finalement il n'y a pas grand chose à retenir. `noexcept` permet certaines optimisations, mais appliqué n'importe comment il est source de bug. Si vous n'êtes pas sûr, ne l'utilisez pas.

Cependant, comme cela influence les conteneurs de la STL, il faut impérativement penser à le mettre sur le constructeur de déplacement.
