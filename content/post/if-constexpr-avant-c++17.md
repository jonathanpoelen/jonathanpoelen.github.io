---
title: "if constexpr avant C++17"
#description: ""
date: 2015-09-21T00:33:38+01:00
lastmod: 2018-03-04T23:14:38+01:00
slug: "if-constexpr-avant-cpp17"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: false
ghcommentid: 0
---

Le but de {{<hi cpp "if constexpr"/>}} est d'interpréter le code uniquement si celui-ci respecte la condition. Il doit être syntaxiquement valide, mais n'a pas l'obligation de pouvoir être compilé.

Plutôt étrange, n'est-ce pas ? Cette propriété se révèle pourtant fort pratique dans les fonctions templates.

Prenons comme exemple une fonction `invoke` qui s'utilise de 3 manières:

- foncteur + paramètres
- fonction membre + objet + paramètres
- fonction membre + pointeur d'objet + paramètres

```cpp
// x et y sont des std::string
invoke(std::equal_to<>{}, x, y); // foncteur
invoke(&std::string::size, x, y); // fonction membre et référence
invoke(&std::string::size, &x, y); // fonction membre et pointeur
```

En C++17, tout peut se faire en une seule fonction, alors qu'avant C++17, il fallait faire plusieurs surcharges pour les différentes situations et jouer avec `std::enable_if`.

## Implémentation avec if constexpr

```cpp
template<class F, class T, class... Args>
decltype(auto) invoke(F&& f, T&& x, Args&&... args)
{
  if constexpr (std::is_member_function_pointer<std::remove_reference_t<F>>::value) {
    if constexpr (std::is_pointer<std::remove_reference_t<T>>::value) {
      return (static_cast<T&&>(x)->*f)(static_cast<Args&&>(args)...);
    }
    else {
      return (static_cast<T&&>(x).*f)(static_cast<Args&&>(args)...);
    }
  }
  else {
    return static_cast<F&&>(f)(static_cast<T&&>(x), static_cast<Args&&>(args)...);
  }
}

template<class F>
decltype(auto) invoke(F&& f)
{
  return static_cast<F&&>(f)();
}
```

## Simulation de if constexpr en pre-C++17

L'atout principal de {{<hi cpp "if constexpr"/>}} ici est de n'évaluer le code qu'au besoin. Il faut donc un moyen de court-circuiter le flux de code. Le plus simple consiste à faire 2 fonctions, une avec un paramètre de type `std::true_type`, l'autre avec un `std::false_type` qui représente le résultat de la condition et 2 paramètres: `If` et `Else`.

Aussi, pour que les foncteurs `If` et `Else` soient évalués au dernier moment, ils devront prendre et utiliser un paramètre générique ({{<hi cpp "auto"/>}}). Sinon le compilateur va vérifier le code au moment de l'instanciation de la lambda plutôt qu'au moment de son utilisation.

```cpp
struct Identity
{
  template<class T>
  decltype(auto) operator()(T&& x) const noexcept
  { return static_cast<T&&>(x); }
};

template<class If, class Else = int>
decltype(auto) if_constexpr(std::true_type cond, If f, Else = {})
{ return f(Identity{}); }

template<class If, class Else>
decltype(auto) if_constexpr(std::false_type cond, If, Else f)
{ return f(Identity{}); }


template<class F, class T, class... Args>
decltype(auto) invoke(F&& f, T&& x, Args&&... args)
{
  return if_constexpr(std::is_member_function_pointer<std::remove_reference_t<F>>{}, [&](auto) {
    return if_constexpr(std::is_pointer<std::remove_reference_t<T>>{}, [&](auto _) {
      return (_(static_cast<T&&>(x))->*f)(static_cast<Args&&>(args)...);
    }, /* else */ [&](auto _) {
      return (_(static_cast<T&&>(x)).*f)(static_cast<Args&&>(args)...);
    });
  }, /* else */ [&](auto _) {
    return _(static_cast<F&&>(f))(static_cast<T&&>(x), static_cast<Args&&>(args)...);
  });
}
```

## Limitation

Cette version ne supporte pas {{<hi cpp "if else"/>}}, demande d'utiliser `_` "là où il faut" et est syntaxiquement plus lourde.
Mais la véritable limitation réside dans l'appel même d'une fonction qui ne propage pas l'usage de {{<hi cpp "break"/>}}, {{<hi cpp "continue"/>}} et {{<hi cpp "return"/>}}.
