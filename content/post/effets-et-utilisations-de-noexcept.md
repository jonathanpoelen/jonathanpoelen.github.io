---
title: "Effets et utilisations de noexcept"
#thumbnail: ""
#description: ""
date: 2019-08-18T18:09:32+02:00
#lastmod: 2019-08-18T18:09:32+02:00
slug: "effets-et-utilisations-de-noexcept"
#toc: false
#tags: [ ]
aliases: []
categories: [ "c++" ]
draft: true
ghcommentid: 0
expire: 2028
---

## Spécificateur et opérateur noexcept

void foo() noexcept(f()); si f constexpr
void foo() noexcept(noexcept(f())); sinon

https://en.cppreference.com/w/cpp/language/noexcept_spec
https://en.cppreference.com/w/cpp/language/noexcept

noexcept
noexcept(bool)
noexcept(expr)

noexcept(noexcept(false)) -> noexcept(true)
noexcept(noexcept(true))  -> noexcept(true)
         ^-------
         operator

noexcept sur ctor, dtor, op=, swap
-> conséquence sur conteneur/swap -> copie en place de déplacement

si exception: std::terminate 

comparaison de noexcept sur un binaire, si implém dans
 - .h
 - .cpp avec et sans lto
 - bibliothèque statique/dynamique

noexcept et méta prog: branchement à la compile (usage... ?)

noexcept dans une signature de fonction (/membre). 
  C++17: est dans le type de la fonction

noexcept et lambda -> implicite (gcc) ou pas (bug clang?). msvc ?
  c++17: https://en.cppreference.com/w/cpp/language/lambda
  If the closure object's operator() has a non-throwing exception specification, then the pointer returned by this function has the type pointer to noexcept function. 
  (vérifier la norme pour l'implicit exception specification)

dtor avec: base noexcept, dérivée noexcept(false): mal formé si virtual

note sur throw():
  Non-throwing dynamic exception specification (unlike noexcept(true) guarantees stack unwinding and may call std::unexpected) 
  17: = noexcept(true) + deprecated
  20: del
  noexcept is an improved version of throw(), which is deprecated in C++11. Unlike pre-C++17 throw(), noexcept will not call std::unexpected and may or may not unwind the stack, which potentially allows the compiler to implement noexcept without the runtime overhead of throw(). As of C++17, throw() is redefined to be an exact equivalent of noexcept(true). 
  supprimé en 20

throw (3) until 17

std::move_if_noexcept
std::is_nothrow_...
