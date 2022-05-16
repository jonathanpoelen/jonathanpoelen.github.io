// g++ -std=c++14 -fsyntax-only decay.cpp

#include <type_traits>

//BEGIN proto add_const/lvalue
// `C` pour continuation

template<class C>
struct add_const;

template<class C>
struct add_lvalue_reference;

template<class C>
using add_const_lvalue_reference = add_const<add_lvalue_reference<C>>; // un chaînage
//END proto add_const/lvalue


//BEGIN impl add_const/lvalue
// Cette implémentation de call est volontairement simplifiée
// et ne fonctionne pas pour tous F
template<class F, class... Ts>
using call = typename F::template f<Ts...>;

template<class C>
struct add_const
{
  template<class T>
  using f = call<C, T const>;
};

template<class C>
struct add_lvalue_reference
{
  template<class T>
  using f = call<C, T&>;
};
//END impl add_const/lvalue

//BEGIN f_term
struct identity
{
  template<class T>
  using f = T;
};

template<class...>
class list {};

struct listify
{
  template<class... Ts>
  using f = list<Ts...>;
};
//END f_term

//BEGIN if
namespace detail
{
  template<bool B>
  struct conditional;
}

template<class P, class TC, class FC>
struct if_
{
  template<class... Ts>
  using f = call<call<detail::conditional<call<P, Ts...>::value>, TC, FC>, Ts...>;
};

template<template<class...> class F, class C = identity>
struct cfl
{
  template<class... Ts>
  using f = call<C, F<Ts...>>;
};

template<template<class...> class F, class C = identity>
struct cfe
{
  template<class... Ts>
  using f = call<C, typename F<Ts...>::type>;
};


namespace detail
{
  template<bool B>
  struct conditional
  {
    template<class T, class U> using f = T;
  };

  template<>
  struct conditional<false>
  {
    template<class T, class U> using f = U;
  };
}
//END if

namespace detail
{
  // std::add_pointer est beaucoup plus complexe que simplement `type = T*`
  template<class T>
  struct add_pointer_impl
  {
    using type = T*;
  };
}

//BEGIN decay
template<class T>
struct decay
{
  using type = call<
    if_<
      cfl<std::is_array>,
      cfe<std::remove_extent, cfe<detail::add_pointer_impl>>,
      if_<
        cfl<std::is_function>,
        cfe<std::add_pointer>,
        cfe<std::remove_cv>
      >
    >,
    std::remove_reference_t<T>
  >;
};
//END decay

// devrait être call<decay<C>, T>
template<class T>
using decay_t = typename decay<T>::type;

template<class T> struct t{};

inline void test()
{
  #define TEST(type) t<std::decay_t<type>>{} = t<decay_t<type>>{}
  TEST(int);
  TEST(int&);
  TEST(int&&);

  TEST(int const);
  TEST(int const&);
  TEST(int const&&);

  TEST(int[2]);
  TEST(int(&)[2]);

  TEST(int(void));
}

// /usr/bin/time --format="%Es - %MK" g++ -std=c++14 a.cpp -fsyntax-only -DNAMESPACE=stdcond -DTO_TYPE='I[2]' -DCOUNT=600

#if defined(NAMESPACE) && defined(TO_TYPE)
#ifndef COUNT
# define COUNT 600
#endif
#include <utility>

namespace stdcond
{
  template<class T>
  struct decay
  {
    using unref = std::remove_reference_t<T>;
    using type = std::conditional_t<
      std::is_array<unref>::value,
      std::remove_extent_t<unref>*,
      std::conditional_t<
        std::is_function<unref>::value,
        std::add_pointer_t<unref>,
        std::remove_cv_t<unref>
      >
    >;
  };
}

template<class... I>
using mk_list_ = list<typename NAMESPACE::decay<TO_TYPE>::type...>;

template<int> class i{};

template<class>
struct mk_list;

template<std::size_t... I>
struct mk_list<std::index_sequence<I...>>
{
  using type = mk_list_<i<I>...>;
};
mk_list<std::make_index_sequence<COUNT>>::type l;
#endif
