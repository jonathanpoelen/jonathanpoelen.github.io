---
title: "SFINAE"
#thumbnail: ""
#description: ""
date: 2020-04-07T09:29:25+02:00
#lastmod: 2020-04-07T09:29:25+02:00
#slug: "a-site-page"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
expire: 2038
---

[SFINAE](https://en.cppreference.com/w/cpp/language/sfinae) (Substitution Failure Is Not An Error) est un mécanisme du compilateur pour ignorer certaines instanciations de fonction ou de classe qui ne compilent pas, sans pour autant émettre une erreur de compilation.

Pour comprendre pleinement le mécanisme derrière, il faut assimiler le principe de substitution appliquée par le compilateur. Lorsqu'une expression dépend d'un **paramètre template**, le compilateur va évaluer l'expression en la substituant par le type ou la valeur de l'expression. Par exemple

```cpp
struct A { using type = int; };

template<class T>
void foo(typename T::type value);

foo<A>(3);
```

À l'appel de {{<hi cpp "foo<A>"/>}}, le compilateur remplace {{<hi cpp "typename T::type"/>}} par {{<hi cpp "A::type"/>}}, qui correspond ici à {{<hi cpp "int"/>}}. C'est la substitution. Lorsque le compilateur n'arrive pas à faire cette substitution -- car par exemple il n'y a pas de membre `type` -- il va chercher une autre fonction {{<hi cpp "foo<A>"/>}} qui pourrait être utilisée.

```cpp
struct B { using value_type = int; };

template<class T>
void foo(typename T::type value); // B::type n'existe pas, cette fonction est ignorée

template<class T>
void foo(typename T::value_type value);

foo<B>(3);
```

Bien sûr, si le compilateur peut utiliser les 2 fonctions `foo` précédentes, il y aura ambiguïté et finalement une erreur.

```cpp
struct C
{
  using value_type = int;
  using type = int;
};

foo<C>(3);
```

```cpp
test.cpp:15:3: error: call to 'foo' is ambiguous
  foo<C>(3);
  ^~~~~~
test.cpp:2:6: note: candidate function [with T = C]
void foo(typename T::type value);
     ^
test.cpp:5:6: note: candidate function [with T = C]
void foo(typename T::value_type value);
     ^
```

Il faut bien comprendre que SFINAE n'est pas un mécanisme pour simplifier les messages d'erreur, ni pour les ignorer. Le résultat est même plutôt inverse, à un certain point, il est difficile de savoir pourquoi une fonction est utilisée à la place d'une autre, le compilateur ne donnant aucun diagnostic. Les erreurs sont plus verbeuses -- l'ensemble des prototypes sont listés -- et les ambiguïtés difficiles à corriger sans ajouter de la complexité.

Plus le nombre de prototypes liés à un nom de fonction croît, plus le code devient difficile et les erreurs -- principalement d'ambiguïtés -- nombreuses. Il faut bien réfléchir à la manière de s'y prendre pour ne pas être happé par le code avec des prototypes à rallonge à ne plus savoir quoi en faire. Le but de cet article est de présenter différentes solutions pour exploiter SFINAE sans être enfermé dans un usage unique qui se limite souvent à la superposition de condition dans un {{<hi cpp "std::enable_if_t"/>}}.


## Dépendance de type/valeur

Le SFINAE repose entièrement sur les valeurs ou types template, on parle de value-dependent ou type-dependent (je ne ferais pas de distinction entre les 2). Tout ce qui n'est pas lié à une valeur template sera évalué automatiquement par le compilateur, que l'expression soit ou non dans une classe ou une fonction template.

Par exemple:

```cpp
struct A { using type = int; };

template<class T>
int foo(T x, A::type y) // A::type doit exister, car le type n'est pas type-dependent
                        // Si A::type n'existe pas, il y aura une erreur
                        // même si foo() n'est pas utilisée
{
  return x + 1; // Cette expression dépend de x qui est template,
                // une erreur ne pourra survenir qu'au moment de l'instanciation de foo()
  return ""; // cette expression n'a aucune dépendance -> il devrait y avoir une erreur
}
```

Si le compilateur respecte scrupuleusement la norme et sans la moindre utilisation de {{<hi cpp "foo()"/>}}, tout ce qui n'a pas de dépendance doit être évalué au plus tôt et systématiquement.

- le type {{<hi cpp "A::type"/>}} doit exister
- le second retour ne doit pas compiler

Étrangement, du moment que la fonction n'est pas utilisée, gcc ne vérifie pas si le retour est convertible vers celui de la fonction, mais demande quand même une expression valide, alors que msvc ne fait aucune vérification au sein de la fonction. Par contre clang indique qu'une chaîne de caractères n'est pas convertible en int.

L'évaluation systématique des types indépendants explique pourquoi le code ci-dessous donne toujours une erreur.

```cpp
if constexpr (xxx)
{}
else
{
  static_assert(false); // toujours évalué -> erreur de compilation
}
```

On touche ici au contexte d'évaluation: le prototype, suivi du corps de fonction. Le second est évalué différemment selon les compilateurs, mais cela ne cause pas réellement de problème, car les erreurs apparaissent au moment de l'utilisation de la fonction.

Quand on parle de contexte, on parle aussi de sous-contexte. Dans le cas de SFINAE, la dépendance d'un contexte au contexte parent **ne peut pas être attrapé**, ce qui résultera dans tous les cas à une erreur de compilation. Le corps d'une fonction est un exemple, un autre concerne les membres d'une classe:

```cpp
struct A
{
  using type = int;
};

struct B
{};

template<class T>
struct C // contexte principal (instanciation de la structure)
{
  using type = typename T::type; // sous-contexte,
                                 // T::type dépend de T du contexte parent
};

template<class T, class = typename T::type>
constexpr int foo(int)
{ return 1; }

template<class T>
constexpr int foo(char)
{ return 2; }

static_assert(foo<A>(0) == 1);
static_assert(foo<B>(0) == 2);
static_assert(foo<C<A>>(0) == 1);
static_assert(foo<C<B>>(0) == 2); // erreur
```

```cpp
test.cpp:12:28: error: no type named 'type' in 'B'
  using type = typename T::type; // sous-contexte,
               ~~~~~~~~~~~~^~~~
test.cpp:16:36: note: in instantiation of template class 'C<B>' requested here
template<class T, class = typename T::type>
                                   ^
```

Pour que `C` fonctionne avec SFINAE, il faut une spécialisation de `C` sans le membre `type`. Cela requière un développement spécifique qui rend le support de SFINAE quelquefois difficile à mettre en place.


## Emplacement des expressions pour le SFINAE

Partout où il est possible de mettre un type ou une valeur est propice à la substitution: paramètre template, contenu de noexcept, decltype, sizeof et même les paramètres par défaut. Quel que soit l'emplacement, si le compilateur ne trouve pas une expression qu'il peut compiler, il va simplement ignorer la fonction ou l'instanciation.

Généralement, pour vérifier une expression, le plus simple est d'ajouter un paramètre de template initialisé avec l'expression qui doit être valide ou passer par {{<hi cpp "decltype"/>}}. Pour ce dernier, une petite astuce pour ajouter plusieurs expressions consiste à les séparer par des virgules et entourer le tout d'une paire de parenthèse:

```cpp
template<..., class = decltype(...)> // ici ou
auto f(...) -> decltype(((void)expr1, (void)(expr2), expr3));
                     // ^ pour que decltype ne voit qu'une expression
                     //  ^ le cast en void permet d'inhiber d'éventuelle surcharge de ,
                     //             ^ la virgule pour séparer chacune des expressions
                     //                              ^ l'expression pour le type de retour
```


## Technique et usage du SFINAE

À partir de maintenant, je vais présenter les techniques qui me viennent à l'esprit en me basant si possible sur des éléments de la STL pour des exemples pratiques. Les plus curieux pourront comparer avec l'implémentation de la STL qu'ils utilisent ([msvc](https://github.com/microsoft/STL/), [clang (libc++)](https://github.com/llvm-mirror/libcxx) et [gcc (libstdc++)](https://github.com/gcc-mirror/gcc/tree/master/libstdc%2B%2B-v3) pour les plus connues).


### Sélection de fonction à travers un type membre

Dans ce premier exemple, nous allons voir comment implémenter {{<hi cpp "std::make_unique"/>}}. Pour rappel, il existe 3 versions:

```cpp
make_unique<T>(args...) -> unique_ptr<T>;
make_unique<T[]>(size) -> unique_ptr<T[]>; // arrays of unknown bound
make_unique<T[N]>(args...) = delete; // arrays of known bound
```

Comme le premier type template influence les paramètres et le type de retour, le plus simple est un trait qui va sélectionner le bon prototype. Contrairement au trait que l'on rencontre habituellement, celui-ci va définir un type membre différent pour chaque spécialisation utilisée par un prototype. Ainsi, seul un prototype pourra être utilisé pendant que les autres ne trouveront pas le membre attendu. Cette implémentation est tout droit sortie des STLs.

```cpp
namespace detail
{
  template<class T>
  struct make_unique_select
  {
    using single_object = std::unique_ptr<T>;
  };

  template<class T>
  struct make_unique_select<T[]>
  {
    using array = std::unique_ptr<T[]>;
  };

  template<class T, std::size_t N>
  struct make_unique_select<T[N]>
  {
    struct invalid_type {};
  };
}

template<class T, class... Args>
typename detail::make_unique_select<T>::single_object
make_unique(Args&&... args)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
typename detail::make_unique_select<T>::array
make_unique(std::size_t num)
{
  return unique_ptr<T>(new std::remove_extent_t<T>[num]());
}

template<class T, class... Args>
typename detail::make_unique_select<T>::invalid_type
make_unique(Args&&...) = delete;
```

Si le C++ supportait la spécialisation partielle de fonction, on pourrait se passer de {{<hi cpp "detail::make_unique_select"/>}}. Par contre, comme l'implémentation de `make_unique` dépend uniquement d'un type template qui doit être explicitement mis, on peut simplifier le code grâce aux variables template et en déplaçant l'implémentation des fonctions dans les spécialisations de structure.

```cpp
namespace detail
{
  template<class T>
  struct make_unique_impl
  {
    template<class... Args>
    std::unique_ptr<T> operator()(Args&&... args) const
    {
       return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
  };

  template<class T>
  struct make_unique_impl<T[]>
  {
    std::unique_ptr<T[]> operator()(std::size_t num) const
    {
      return std::unique_ptr<T[]>(new std::remove_extent_t<T>[num]());
    }
  };

  template<class T, std::size_t N>
  struct make_unique_impl<T[N]>
  {
    template<class... Args>
    struct invalid_type operator()(Args&&...) const = delete;
  };
}

template<class T>
constexpr detail::make_unique_impl<T> make_unique {};
```

Ce qui revient à des bêtes spécialisations de template.


### Condition dans une spécialisation template

Grâce à un paramètre supplémentaire, il est possible d'ajouter des conditions dans une spécialisation pour y ajouter des contraintes. Voici un exemple simplifié avec une implémentation de {{<hi cpp "std::is_convertible"/>}}.

```cpp
template<class From, class To, class = void>
struct is_convertible : std::false_type
{};

template<class From, class To>
struct is_convertible<From, To,
  // on utilise le paramètre "fantôme",
  // mais le type final de l'expression doit être le même que la valeur par défaut
  class = decltype(
    // fonction créée à la volée qui prend un paramètre To
    // qui échouera si From n'est pas convertible
    std::declval<void(&)(To)>()(std::declval<From>())
  )
> : std::true_type
{};
```

Cette technique permet de n'instancier un type que si tous les prérequis sont acceptés. Au début de l'article, j'ai présenté un exemple de classe où {{<hi cpp "T::type"/>}} (avec {{<hi cpp "T = C<B>"/>}}) donne une erreur, car le sous-membre (`type`) dépend d'un contexte parent. Voici comment on peut palier à ce problème:

```cpp
template<class T, class = void>
struct C
{
  // par défaut aucun membre
};

// si T::type existe bien, cette spécialisation sera utilisée
template<class T>
struct C<T, std::void_t<typename T::type>>
{
  using type = typename T::type;
};
```

Ceci fonctionne pour des types, mais les compilateurs sont plus capricieux avec des valeurs. Par exemple, une spécialisation sur des valeurs divisibles par 16 comme ci-dessus donne une erreur avec gcc, msvc et icc. Étrangement, seul clang l'accepte.

```cpp
template<std::size_t N, bool = false>
struct is_mod16 : std::false_type
{};

template<std::size_t N>
struct is_mod16<N, (N % 16 == 0)> : std::true_type // erreur
{};
```

Faute de mieux, le plus simple est de passer par {{<hi cpp "std::enable_if_t<N % 16 == 0>"/>}} et prendre un type comme second paramètre de template.


### Jouer avec les conversions implicites

Il est possible d'ajouter un paramètre dans une fonction et jouer avec les conversions implicites pour prioriser l'ordre d'appel.

```cpp
template<class T, class = typename T::type>
constexpr int foo(int) { return 1; }

template<class T, class = typename T::value_type>
constexpr int foo(char) { return 2; }

struct A
{
  using type = int;
};

struct B
{
  using value_type = int;
};

struct C : A, B
{};

static_assert(foo<A>(42) == 1);
static_assert(foo<B>(42) == 2);
// les 2 fonctions sont valides, mais T::type est prioritaire car 42 est un int
static_assert(foo<C>(42) == 1);
```

On peut même ajouter autant de paramètres que nécessaire pour avoir une chaîne de priorité. Par exemple, on pourrait faire une fonction {{<hi cpp "front()"/>}} qui utilise en priorité la fonction {{<hi cpp "front(cont)"/>}} dans le namespace de l'objet (règles d'[ADL](https://en.cppreference.com/w/cpp/language/adl)), sinon `cont.front()` et en dernier recours {{<hi cpp "*begin(cont)"/>}}.

{{<fhi "sfinae/front.cpp">}}

### Les variadiques de la dernière chance

Il existe en C des paramètres variadiques dont l'usage est limité aux types de base et aux pointeurs. Ce genre de paramètre à une particularité intéressante: il n'est pris en compte qu'à la seule condition qu'aucune autre fonction ne pourrait correspondre. Il a donc une priorité inférieure à la conversion et ne rentre pas en conflit avec. On pourrait réécrire {{<hi cpp "std::is_convertible"/>}} comme suit

```cpp
template<class From, class To>
struct is_convertible_impl
{
  std::true_type test(To);
  std::false_type test(...);
};

template<class From, class To>
using is_convertible = decltype(is_convertible_impl<From, To>::test(std::declval<From>()));
```


### Rendre dépendant un type indépendant

Cela peut paraître idiot, mais pour rendre dépendant un type indépendant du contexte, il suffit qu'il traverse un type dépendant.

Voici un exemple pour avoir le plus proche d'un {{<hi cpp "if constexpr () {} else static_assert(false)"/>}}:

```cpp
constexpr auto fn_identity = [](auto&& x) -> decltype(auto) {
  return static_cast<decltype(x)&&>(x);
};

template<class>
inline auto dependent_identity = fn_identity;

template<class T>
void foo(T const& x)
{
  if constexpr (std::is_array_v<T>)
  {
    // ...
  }
  else
  {
    // _ depend de T, mais est toujours égal à fn_identity
    auto _ = dependent_identity<T>;
    // _(false) retourne false,
    // mais la valeur est indirectement dépendante de T
    static_assert(_(false));
    // on notera que même si _ n'est pas une variable constexpr
    // le résultat de _(false) l'est :)
  }
}

// on peut aussi utiliser la variable x plutôt que T, avec quelque chose comme
template<class T, class U>
constexpr first(T x, U const&)
{
  return x;
}

//...
static_assert(first(false, x));
```

Cela s'applique aussi au membre d'une classe en ajoutant un template avec un paramètre par défaut qui ne sera jamais modifié par l'extérieur.

```cpp
template<class T>
struct A
{
  template<class U = T>
  using type = typename U::type;
};

// son écriture en est par contre fortement alourdie:
A<T>::type<>
A<T>::template type<> // dans une fonction/classe template
```

Ou sur une fonction membre:

```cpp
template<class T>
struct A
{
  // sans ce template, le type T doit obligatoirement avoir une fonction foo()
  // sinon, l'instanciation de A ne pourra pas se faire et A<int> donnerait
  // request for member ‘foo’ in ‘std::declval<int&>()’, which is of non-class type ‘int’
  template<class U = T>
  decltype(std::declval<U&>().foo()) foo();
};
```


## C++20 et concept

Depuis C++20, il existe un nouvel outil pour exploiter le SFINAE: [les concepts](https://en.cppreference.com/w/cpp/language/constraints). Les concepts sont des contraintes ajoutées aux types qui doivent être vérifiées pour que le compilateur accepte d'utiliser la fonction ou d'instancier le type. Cela simplifie l'utilisation du SFINAE et rend le code beaucoup plus lisible. La fonctionnalité étant récente, la plupart des compilateurs ne la supporte pas encore.
