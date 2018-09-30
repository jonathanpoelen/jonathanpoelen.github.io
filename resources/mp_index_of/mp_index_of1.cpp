// -fsyntax-only -DTESTS
// -fsyntax-only -DNAMESPACE=

#include <type_traits>
#include <utility>


//BEGIN recursive_ternary
namespace recursive_ternary
{
namespace detail
{
  template<class T, class... Ts>
  struct _index_of : std::integral_constant<int, 1> {};

  template<class T, class First, class... Rest>
  struct _index_of<T, First, Rest...>
  : std::integral_constant<int, std::is_same_v<T, First>
    ? 0 : _index_of<T, Rest...>::value + 1> {};
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<T, Ts...>::value > sizeof...(Ts))
    ? -1 : detail::_index_of<T, Ts...>::value>;
}
//END recursive_ternary


//BEGIN recursive_indexed
namespace recursive_indexed
{
namespace detail
{
  template<int i, class T, class... Ts>
  struct _index_of : std::integral_constant<int, -1> {};

  template<int i, class T, class First, class... Rest>
  struct _index_of<i, T, First, Rest...>
  : _index_of<i+1, T, Rest...> {};

  template<int i, class T, class... Rest>
  struct _index_of<i, T, T, Rest...>
  : std::integral_constant<int, i> {};
}

template<class T, class... Ts>
using mp_index_of = typename detail::_index_of<0, T, Ts...>::type;
}
//END recursive_indexed


//BEGIN recursive
namespace recursive
{
namespace detail
{
  template<class T, class... Ts>
  struct _index_of : std::integral_constant<int, 1> {};

  template<class T, class First, class... Rest>
  struct _index_of<T, First, Rest...>
  : std::integral_constant<int, 1+_index_of<T, Rest...>::value> {};

  template<class T, class... Rest>
  struct _index_of<T, T, Rest...>
  : std::integral_constant<int, 0> {};
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<T, Ts...>::value > sizeof...(Ts))
    ? -1 : detail::_index_of<T, Ts...>::value>;
}
//END recursive


//BEGIN recursive_bool
namespace recursive_bool
{
template<class T, class... Ts>
using mp_index_of = recursive::mp_index_of<std::true_type, typename std::is_same<T, Ts>::type...>;
}
//END recursive_bool


//BEGIN recursive_bool_ternary
namespace recursive_bool_ternary
{
template<class T, class... Ts>
using mp_index_of = recursive_ternary::mp_index_of<std::true_type, typename std::is_same<T, Ts>::type...>;
}
//END recursive_bool_ternary


//BEGIN recursive_bool2
namespace recursive_bool2
{
namespace detail
{
  template<class, class... Rest>
  struct _index_of
  : std::integral_constant<int, 1+_index_of<Rest...>::value>
  {};

  template<class... Rest>
  struct _index_of<std::true_type, Rest...>
  : std::integral_constant<int, 0> {};
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<typename std::is_same<T, Ts>::type..., std::true_type>::value >= sizeof...(Ts))
    ? -1 : detail::_index_of<typename std::is_same<T, Ts>::type..., std::true_type>::value>;
}
//END recursive_bool2

//BEGIN recursive_bool3
namespace recursive_bool3
{
namespace detail
{
  template<bool, bool... Rest>
  struct _index_of
  : std::integral_constant<int, 1+_index_of<Rest...>::value>
  {};

  template<bool... Rest>
  struct _index_of<true, Rest...>
  : std::integral_constant<int, 0> {};
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<std::is_same<T, Ts>::value..., true>::value >= sizeof...(Ts))
    ? -1 : detail::_index_of<std::is_same<T, Ts>::value..., true>::value>;
}
//END recursive_bool3


//BEGIN pack8
namespace pack8
{
namespace detail
{
  template<bool Ok, class T,
           class U0 = T, class U1 = T, class U2 = T, class U3 = T, class U4 = T, class U5 = T, class U6 = T, class U7 = T,
           class... Rest>
  struct _index_of
  : std::integral_constant<int, (std::is_same_v<T, U0> ? 0 :
                                 std::is_same_v<T, U1> ? 1 :
                                 std::is_same_v<T, U2> ? 2 :
                                 std::is_same_v<T, U3> ? 3 :
                                 std::is_same_v<T, U4> ? 4 :
                                 std::is_same_v<T, U5> ? 5 :
                                 std::is_same_v<T, U6> ? 6 :
                                 std::is_same_v<T, U7> ? 7 : 8)
    + _index_of<std::is_same_v<T, U0> ||
                std::is_same_v<T, U1> ||
                std::is_same_v<T, U2> ||
                std::is_same_v<T, U3> ||
                std::is_same_v<T, U4> ||
                std::is_same_v<T, U5> ||
                std::is_same_v<T, U6> ||
                std::is_same_v<T, U7>
    , T, Rest...>::value> {};

  template<class T, class U0, class U1, class U2, class U3, class U4, class U5, class U6, class U7, class... Rest>
  struct _index_of<true, T, U0, U1, U2, U3, U4, U5, U6, U7, Rest...>
  : std::integral_constant<int, 0> {};

  template<class T, class U0, class U1, class U2, class U3, class U4, class U5, class U6, class U7>
  struct _index_of<false, T, U0, U1, U2, U3, U4, U5, U6, U7>
  : std::integral_constant<int, std::is_same_v<T, U0> ? 0 :
                                std::is_same_v<T, U1> ? 1 :
                                std::is_same_v<T, U2> ? 2 :
                                std::is_same_v<T, U3> ? 3 :
                                std::is_same_v<T, U4> ? 4 :
                                std::is_same_v<T, U5> ? 5 :
                                std::is_same_v<T, U6> ? 6 :
                                std::is_same_v<T, U7> ? 7 : 8> {};
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<false, T, Ts...>::value >= sizeof...(Ts))
    ? -1 : detail::_index_of<false, T, Ts...>::value>;
}
//END pack8


//BEGIN pack8_bool
namespace pack8_bool
{
namespace detail
{
  using Ok = std::true_type;

  template<bool,
           class U0 = Ok, class U1 = Ok, class U2 = Ok, class U3 = Ok, class U4 = Ok, class U5 = Ok, class U6 = Ok, class U7 = Ok,
           class... Rest>
  struct _index_of
  : std::integral_constant<int, (U0::value ? 0 :
                                 U1::value ? 1 :
                                 U2::value ? 2 :
                                 U3::value ? 3 :
                                 U4::value ? 4 :
                                 U5::value ? 5 :
                                 U6::value ? 6 :
                                 U7::value ? 7 : 8)
    + _index_of<U0::value ||
                U1::value ||
                U2::value ||
                U3::value ||
                U4::value ||
                U5::value ||
                U6::value ||
                U7::value
    , Rest...>::value> {};

  template<class U0, class U1, class U2, class U3, class U4, class U5, class U6, class U7, class... Rest>
  struct _index_of<true, U0, U1, U2, U3, U4, U5, U6, U7, Rest...>
  : std::integral_constant<int, 0> {};
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<false, typename std::is_same<T, Ts>::type...>::value >= sizeof...(Ts))
    ? -1 : detail::_index_of<false, typename std::is_same<T, Ts>::type...>::value>;
}
//END pack8_bool


//BEGIN pack8_bool2
namespace pack8_bool2
{
namespace detail
{
  using Ok = std::true_type;

  template<class,
           class U0 = Ok, class U1 = Ok, class U2 = Ok, class U3 = Ok, class U4 = Ok, class U5 = Ok, class U6 = Ok, class U7 = Ok,
           class... Rest>
  struct _index_of
  : std::integral_constant<int, (U0::value ? 0 :
                                 U1::value ? 1 :
                                 U2::value ? 2 :
                                 U3::value ? 3 :
                                 U4::value ? 4 :
                                 U5::value ? 5 :
                                 U6::value ? 6 :
                                 U7::value ? 7 : 8)
    + _index_of<std::integral_constant<bool, U0::value ||
                                             U1::value ||
                                             U2::value ||
                                             U3::value ||
                                             U4::value ||
                                             U5::value ||
                                             U6::value ||
                                             U7::value>
    , Rest...>::value> {};

  template<class U0, class U1, class U2, class U3, class U4, class U5, class U6, class U7, class... Rest>
  struct _index_of<std::true_type, U0, U1, U2, U3, U4, U5, U6, U7, Rest...>
  : std::integral_constant<int, 0> {};
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<std::false_type, typename std::is_same<T, Ts>::type...>::value >= sizeof...(Ts))
    ? -1 : detail::_index_of<std::false_type, typename std::is_same<T, Ts>::type...>::value>;
}
//END pack8_bool2


//BEGIN pack8_alias
namespace pack8_alias
{
namespace detail
{
  template<bool>
  struct _index_of_impl
  {
    template<class T, class U0 = T, class U1 = T, class U2 = T,  class U3 = T,  class U4 = T,  class U5 = T,  class U6 = T,  class U7 = T,
             class U8 = T, class U9 = T, class U10 = T, class U11 = T, class U12 = T, class U13 = T, class U14 = T, class U15 = T,
             class... Rest>
    using type = std::integral_constant<int,
      8 + (_index_of_impl<std::is_same<T, U8>::value
                       || std::is_same<T, U9>::value
                       || std::is_same<T, U10>::value
                       || std::is_same<T, U11>::value
                       || std::is_same<T, U12>::value
                       || std::is_same<T, U13>::value
                       || std::is_same<T, U14>::value
                       || std::is_same<T, U15>::value
    >::template type<T, U8, U9, U10, U11, U12, U13, U14, U15, Rest...>::value)>;
  };

  template<>
  struct _index_of_impl<true>
  {
    template<class T, class U0, class U1, class U2, class U3, class U4, class U5, class U6, class U7, class... Rest>
    using type = std::integral_constant<int, std::is_same<T, U0>::value ? 0 :
                                             std::is_same<T, U1>::value ? 1 :
                                             std::is_same<T, U2>::value ? 2 :
                                             std::is_same<T, U3>::value ? 3 :
                                             std::is_same<T, U4>::value ? 4 :
                                             std::is_same<T, U5>::value ? 5 :
                                             std::is_same<T, U6>::value ? 6 :
                                             std::is_same<T, U7>::value ? 7 : 8>;
  };


  template<class T, class U0 = T, class U1 = T, class U2 = T, class U3 = T, class U4 = T, class U5 = T, class U6 = T, class U7 = T,
           class... Rest>
  struct _index_of : _index_of_impl<std::is_same<T, U0>::value
                                 || std::is_same<T, U1>::value
                                 || std::is_same<T, U2>::value
                                 || std::is_same<T, U3>::value
                                 || std::is_same<T, U4>::value
                                 || std::is_same<T, U5>::value
                                 || std::is_same<T, U6>::value
                                 || std::is_same<T, U7>::value
  >::template type<T, U0, U1, U2, U3, U4, U5, U6, U7, Rest...> {};
}


template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<T, Ts...>::value >= sizeof...(Ts))
    ? -1 : detail::_index_of<T, Ts...>::value>;
}
//END pack8_alias


//BEGIN pack8_alias_bool
namespace pack8_alias_bool
{
namespace detail
{
  using Ok = std::true_type;

  template<bool>
  struct _index_of_impl
  {
    template<class U0 = Ok, class U1 = Ok, class U2 = Ok,  class U3 = Ok,  class U4 = Ok,  class U5 = Ok,  class U6 = Ok,  class U7 = Ok,
             class U8 = Ok, class U9 = Ok, class U10 = Ok, class U11 = Ok, class U12 = Ok, class U13 = Ok, class U14 = Ok, class U15 = Ok,
             class... Rest>
    using type = std::integral_constant<int,
      8 + (_index_of_impl<U8::value
                       || U9::value
                       || U10::value
                       || U11::value
                       || U12::value
                       || U13::value
                       || U14::value
                       || U15::value
    >::template type<U8, U9, U10, U11, U12, U13, U14, U15, Rest...>::value)>;
  };

  template<>
  struct _index_of_impl<true>
  {
    template<class U0, class U1, class U2, class U3, class U4, class U5, class U6, class U7, class... Rest>
    using type = std::integral_constant<int, U0::value ? 0 :
                                             U1::value ? 1 :
                                             U2::value ? 2 :
                                             U3::value ? 3 :
                                             U4::value ? 4 :
                                             U5::value ? 5 :
                                             U6::value ? 6 :
                                             U7::value ? 7 : 8>;
  };


  template<class U0 = Ok, class U1 = Ok, class U2 = Ok, class U3 = Ok, class U4 = Ok, class U5 = Ok, class U6 = Ok, class U7 = Ok,
           class... Rest>
  struct _index_of : _index_of_impl<U0::value
                                 || U1::value
                                 || U2::value
                                 || U3::value
                                 || U4::value
                                 || U5::value
                                 || U6::value
                                 || U7::value
  >::template type<U0, U1, U2, U3, U4, U5, U6, U7, Rest...> {};
}


template<class T, class... Ts>
using mp_index_of = std::integral_constant<int,
  (detail::_index_of<typename std::is_same<T, Ts>::type...>::value >= sizeof...(Ts))
    ? -1 : detail::_index_of<typename std::is_same<T, Ts>::type...>::value>;
}
//END pack8_alias_bool


//BEGIN by_indices
namespace by_indices
{
namespace detail
{
  template<class...>
  struct list;

  template<class L>
  struct front;

  template<class T, class... Ts>
  struct front<list<T, Ts...>>
  {
    using type = T;
  };

  template<class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>,
           class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>,
           class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>,
           class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>,
           class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>,
           class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>,
           class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>,
           class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>, class = list<>,
           class... Ls>
  struct _join;

  template<class... _0, class... _1, class... _2, class... _3, class... _4, class... _5, class... _6, class... _7,
           class... _8, class... _9, class... _10, class... _11, class... _12, class... _13, class... _14, class... _15,
           class... _16, class... _17, class... _18, class... _19, class... _20, class... _21, class... _22, class... _23,
           class... _24, class... _25, class... _26, class... _27, class... _28, class... _29, class... _30, class... _31,
           class... _32, class... _33, class... _34, class... _35, class... _36, class... _37, class... _38, class... _39,
           class... _40, class... _41, class... _42, class... _43, class... _44, class... _45, class... _46, class... _47,
           class... _48, class... _49, class... _50, class... _51, class... _52, class... _53, class... _54, class... _55,
           class... _56, class... _57, class... _58, class... _59, class... _60, class... _61, class... _62, class... _63>
  struct _join<list<_0...>, list<_1...>, list<_2...>, list<_3...>, list<_4...>, list<_5...>, list<_6...>, list<_7...>,
               list<_8...>, list<_9...>, list<_10...>, list<_11...>, list<_12...>, list<_13...>, list<_14...>, list<_15...>,
               list<_16...>, list<_17...>, list<_18...>, list<_19...>, list<_20...>, list<_21...>, list<_22...>, list<_23...>,
               list<_24...>, list<_25...>, list<_26...>, list<_27...>, list<_28...>, list<_29...>, list<_30...>, list<_31...>,
               list<_32...>, list<_33...>, list<_34...>, list<_35...>, list<_36...>, list<_37...>, list<_38...>, list<_39...>,
               list<_40...>, list<_41...>, list<_42...>, list<_43...>, list<_44...>, list<_45...>, list<_46...>, list<_47...>,
               list<_48...>, list<_49...>, list<_50...>, list<_51...>, list<_52...>, list<_53...>, list<_54...>, list<_55...>,
               list<_56...>, list<_57...>, list<_58...>, list<_59...>, list<_60...>, list<_61...>, list<_62...>, list<_63...>>
  {
    using type = list<_0..., _1..., _2..., _3..., _4..., _5..., _6..., _7...,
                      _8..., _9..., _10..., _11..., _12..., _13..., _14..., _15...,
                      _16..., _17..., _18..., _19..., _20..., _21..., _22..., _23...,
                      _24..., _25..., _26..., _27..., _28..., _29..., _30..., _31...,
                      _32..., _33..., _34..., _35..., _36..., _37..., _38..., _39...,
                      _40..., _41..., _42..., _43..., _44..., _45..., _46..., _47...,
                      _48..., _49..., _50..., _51..., _52..., _53..., _54..., _55...,
                      _56..., _57..., _58..., _59..., _60..., _61..., _62..., _63...>;
  };

  template<class... _0, class... _1, class... _2, class... _3, class... _4, class... _5, class... _6, class... _7,
           class... _8, class... _9, class... _10, class... _11, class... _12, class... _13, class... _14, class... _15,
           class... _16, class... _17, class... _18, class... _19, class... _20, class... _21, class... _22, class... _23,
           class... _24, class... _25, class... _26, class... _27, class... _28, class... _29, class... _30, class... _31,
           class... _32, class... _33, class... _34, class... _35, class... _36, class... _37, class... _38, class... _39,
           class... _40, class... _41, class... _42, class... _43, class... _44, class... _45, class... _46, class... _47,
           class... _48, class... _49, class... _50, class... _51, class... _52, class... _53, class... _54, class... _55,
           class... _56, class... _57, class... _58, class... _59, class... _60, class... _61, class... _62, class... _63,
           class... Ls>
  struct _join<list<_0...>, list<_1...>, list<_2...>, list<_3...>, list<_4...>, list<_5...>, list<_6...>, list<_7...>,
               list<_8...>, list<_9...>, list<_10...>, list<_11...>, list<_12...>, list<_13...>, list<_14...>, list<_15...>,
               list<_16...>, list<_17...>, list<_18...>, list<_19...>, list<_20...>, list<_21...>, list<_22...>, list<_23...>,
               list<_24...>, list<_25...>, list<_26...>, list<_27...>, list<_28...>, list<_29...>, list<_30...>, list<_31...>,
               list<_32...>, list<_33...>, list<_34...>, list<_35...>, list<_36...>, list<_37...>, list<_38...>, list<_39...>,
               list<_40...>, list<_41...>, list<_42...>, list<_43...>, list<_44...>, list<_45...>, list<_46...>, list<_47...>,
               list<_48...>, list<_49...>, list<_50...>, list<_51...>, list<_52...>, list<_53...>, list<_54...>, list<_55...>,
               list<_56...>, list<_57...>, list<_58...>, list<_59...>, list<_60...>, list<_61...>, list<_62...>, list<_63...>,
               Ls...>
  : _join<list<_0..., _1..., _2..., _3..., _4..., _5..., _6..., _7...,
               _8..., _9..., _10..., _11..., _12..., _13..., _14..., _15...,
               _16..., _17..., _18..., _19..., _20..., _21..., _22..., _23...,
               _24..., _25..., _26..., _27..., _28..., _29..., _30..., _31...,
               _32..., _33..., _34..., _35..., _36..., _37..., _38..., _39...,
               _40..., _41..., _42..., _43..., _44..., _45..., _46..., _47...,
               _48..., _49..., _50..., _51..., _52..., _53..., _54..., _55...,
               _56..., _57..., _58..., _59..., _60..., _61..., _62..., _63...
          >, Ls...> {};

  template<class Ints, class... Ts>
  struct _find_index;

  template<class>
  struct if_
  {
    template<class T, class U>
    using type = U;
  };

  template<>
  struct if_<std::true_type>
  {
    template<class T, class U>
    using type = T;
  };

  template<std::size_t... Ints, class... Is>
  struct _find_index<std::integer_sequence<std::size_t, Ints...>, Is...>
  : front<
      typename _join<
        typename if_<Is>::template type<
          list<std::integral_constant<int, int(Ints)>>,
          list<>
        >...,
        list<std::integral_constant<int, -1>>
      >::type
    > {};
}

template<class T, class... Ts>
using mp_index_of = typename detail::_find_index<
  std::make_index_sequence<sizeof...(Ts)>,
  typename std::is_same<T, Ts>::type...>::type;
}
//END by_indices


//BEGIN loop
namespace loop
{
namespace detail
{
  template<bool... xs>
  constexpr int _index_of()
  {
    bool matches[]{xs..., false};
    for (std::size_t i = 0; i < sizeof...(xs); ++i) {
      if (matches[i]) {
        return int(i);
      }
    }
    return -1;
  }
}

template<class T, class... Ts>
  using mp_index_of = std::integral_constant<int,
    detail::_index_of<std::is_same_v<T, Ts>...>()>;
}
//END loop


//BEGIN loop2
namespace loop2
{
namespace detail
{
  template<class... T>
  constexpr int _index_of(T... xs)
  {
    bool matches[]{xs..., false};
    for (std::size_t i = 0; i < sizeof...(xs); ++i) {
      if (matches[i]) {
        return int(i);
      }
    }
    return -1;
  }
}

template<class T, class... Ts>
  using mp_index_of = std::integral_constant<int,
    detail::_index_of(std::is_same_v<T, Ts>...)>;
}
//END loop2


//BEGIN loop3
namespace loop3
{
namespace detail
{
  template<class T, class... Ts>
  constexpr int _index_of()
  {
    bool matches[]{std::is_same_v<T, Ts>..., false};
    for (std::size_t i = 0; i < sizeof...(Ts); ++i) {
      if (matches[i]) {
        return int(i);
      }
    }
    return -1;
  }
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int, detail::_index_of<T, Ts...>()>;
}
//END loop3


//BEGIN loop4
namespace loop4
{
namespace detail
{
  constexpr int _index_of_impl(bool const* first, bool const* last)
  {
    int i = 0;
    while (first < last && !*first) {
      ++first;
      ++i;
    }
    return first == last ? -1 : i;
  }

  template<class T, class... Ts>
  struct _index_of
  {
    static constexpr bool bools[]{std::is_same_v<T, Ts>..., false};
    using type = std::integral_constant<int, _index_of_impl(bools, bools+sizeof...(Ts))>;
  };
}

template<class T, class... Ts>
using mp_index_of = typename detail::_index_of<T, Ts...>::type;
}
//END loop4


//BEGIN loop_rec
namespace loop_rec
{
namespace detail
{
  template<bool>
  constexpr int _index_of()
  {
    return 0;
  }

  template<bool x, bool y, bool... xs>
  constexpr int _index_of()
  {
    return x ? 0 : _index_of<y, xs...>() + 1;
  }
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int, (
  detail::_index_of<std::is_same_v<T, Ts>..., true>() >= sizeof...(Ts)
    ? -1 : detail::_index_of<std::is_same_v<T, Ts>..., true>())>;
}
//END loop_rec


//BEGIN loop_rec2
namespace loop_rec2
{
namespace detail
{
  constexpr int _index_of()
  {
    return 1;
  }

  template<class... Ts>
  constexpr int _index_of(bool x, Ts... xs)
  {
    return x ? 0 : _index_of(xs...) + 1;
  }
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int, (
  detail::_index_of(std::is_same_v<T, Ts>...) >= sizeof...(Ts)
    ? -1 : detail::_index_of(std::is_same_v<T, Ts>...))>;
}
//END loop_rec2


//BEGIN loop_rec3
namespace loop_rec3
{
namespace detail
{
  template<bool x, bool... xs>
  constexpr int _index_of()
  {
    if constexpr (x) {
      return 0;
    }
    else {
      return _index_of<xs...>() + 1;
    }
  }
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int, (
  detail::_index_of<std::is_same_v<T, Ts>..., true>() >= sizeof...(Ts)
    ? -1 : detail::_index_of<std::is_same_v<T, Ts>..., true>())>;
}
//END loop_rec3


//BEGIN fold
namespace fold
{
namespace detail
{
  template<bool... xs>
  constexpr int _index_of()
  {
    int i = -1;
    (void)((((void)++i, !xs) && ...) && ++i);
    return i >= int(sizeof...(xs)) ? -1 : i;
  }
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int, detail::_index_of<std::is_same_v<T, Ts>...>()>;
}
//END fold


//BEGIN fold2
namespace fold2
{
namespace detail
{
  struct _counter
  {
    int i = -1;

    constexpr bool next(bool x) { ++i; return !x; }
  };

  template<bool... xs>
  constexpr int _index_of()
  {
    _counter counter;
    (void)((counter.next(xs) && ...) && counter.next(false));
    return counter.i >= int(sizeof...(xs)) ? -1 : counter.i;
  }
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<int, detail::_index_of<std::is_same_v<T, Ts>...>()>;
}
//END fold2


//BEGIN test
#ifdef TESTS
template<int x> inline auto i = std::integral_constant<int, x>{};
template<class x> inline auto ix = std::integral_constant<int, x::value>{};

template<template<class, class...> class index_of>
void test()
{
  using _ = char;

  ix<index_of<int>> = i<-1>;
  ix<index_of<int, _>> = i<-1>;
  ix<index_of<int, _, _>> = i<-1>;
  ix<index_of<int, _, _, _, _, _, _>> = i<-1>;
  ix<index_of<int, _, _, _, _, _, _, _>> = i<-1>;
  ix<index_of<int, _, _, _, _, _, _, _, _>> = i<-1>;
  ix<index_of<int, _, _, _, _, _, _, _, _, _>> = i<-1>;
  ix<index_of<int, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _>> = i<-1>;
  ix<index_of<int, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _>> = i<-1>;
  ix<index_of<int, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _>> = i<-1>;

  ix<index_of<int, int>> = i<0>;
  ix<index_of<int, int, int>> = i<0>;
  ix<index_of<int, _, int>> = i<1>;
  ix<index_of<int, _, _, int>> = i<2>;
  ix<index_of<int, _, _, _, _, _, _, int>> = i<6>;
  ix<index_of<int, _, _, _, _, _, _, _, int>> = i<7>;
  ix<index_of<int, _, _, _, _, _, _, _, _, int>> = i<8>;
  ix<index_of<int, _, _, _, _, _, _, _, _, _, int>> = i<9>;
  ix<index_of<int, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, int>> = i<15>;
  ix<index_of<int, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, int>> = i<16>;
  ix<index_of<int, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, int>> = i<17>;

  ix<index_of<int, int>> = i<0>;
  ix<index_of<int, int, _>> = i<0>;
  ix<index_of<int, _, int, _>> = i<1>;
  ix<index_of<int, _, _, int, _>> = i<2>;
  ix<index_of<int, _, int, _, _, _, _, _>> = i<1>;
  ix<index_of<int, _, int, _, _, _, _, _, _>> = i<1>;
  ix<index_of<int, _, int, _, _, _, _, _, _, _>> = i<1>;
  ix<index_of<int, _, int, _, _, _, _, _, _, _, _, _, _, _, _, _>> = i<1>;
  ix<index_of<int, _, int, _, _, _, _, _, _, _, _, _, _, _, _, _, _>> = i<1>;
  ix<index_of<int, _, int, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _>> = i<1>;

  ix<index_of<int, _, _, _, _, _, _, _, int, _>> = i<7>;
  ix<index_of<int, _, _, _, _, _, _, _, int, _, _, _, _, _, _, _>> = i<7>;
  ix<index_of<int, _, _, _, _, _, _, _, int, _, _, _, _, _, _, _, _>> = i<7>;
  ix<index_of<int, _, _, _, _, _, _, _, int, _, _, _, _, _, _, _, _, _>> = i<7>;
}
//END test

//BEGIN test_all
inline void test_all()
{
  test<recursive_ternary::mp_index_of>();
  test<recursive_indexed::mp_index_of>();
  test<recursive::mp_index_of>();
  test<recursive_bool::mp_index_of>();
  test<recursive_bool_ternary::mp_index_of>();
  test<recursive_bool2::mp_index_of>();
  test<recursive_bool3::mp_index_of>();

  test<pack8::mp_index_of>();
  test<pack8_bool::mp_index_of>();
  test<pack8_bool2::mp_index_of>();
  test<pack8_alias::mp_index_of>();
  test<pack8_alias_bool::mp_index_of>();

  test<by_indices::mp_index_of>();

  test<loop::mp_index_of>();
  test<loop2::mp_index_of>();
  test<loop3::mp_index_of>();
  test<loop4::mp_index_of>();

  test<loop_rec::mp_index_of>();
  test<loop_rec2::mp_index_of>();
  test<loop_rec3::mp_index_of>();

  test<fold::mp_index_of>();
  test<fold2::mp_index_of>();
}
//END
#else


#define PP_CAT_I(x, y) x##y
#define PP_CAT(x, y) PP_CAT_I(x, y)
#define FROM PP_CAT(test_, __COUNTER__) =


using x = int;
using _ = char;


# ifdef TEST_EMPTY
using empty = NAMESPACE::mp_index_of<x>;
# endif


//BEGIN ntest
template<class L_1, class Lx, class L_2>
struct make_ntest;

template<std::size_t, class T> using itox = T;

template<std::size_t... L_1, std::size_t... Lx, std::size_t... L_2>
struct make_ntest<
  std::integer_sequence<std::size_t, L_1...>,
  std::integer_sequence<std::size_t, Lx...>,
  std::integer_sequence<std::size_t, L_2...>
> {
  using type = NAMESPACE::mp_index_of<x,
    itox<L_1, _>..., itox<Lx, x>..., itox<L_2, _>... >;
};

// mp_index_of<x, _, _, _, x, x, x, _, _>
//                0-------i--------i2----N
#define NTEST(i, i2, n)                                                         \
  [[maybe_unused]] inline std::integral_constant<int, ((i) == (i2)) ? -1 : (i)> \
    FROM make_ntest<                                                            \
      std::make_index_sequence<(i)>,                                            \
      std::make_index_sequence<(i2)-(i)>,                                       \
      std::make_index_sequence<(n)-(i2)>>::type{}

// x, x, x, x, x
#define NTEST_0(n) NTEST(0, n, n)
// _, _, x, _, _
#define NTEST_1(n) NTEST(n/2, n/2+1, n)
// _, _, x, x, x
#define NTEST_2(n) NTEST(n/2, n, n)
// _, _, _, _, x
#define NTEST_3(n) NTEST(n-1, n, n)
// _, _, _, _, _
#define NTEST_4(n) NTEST(0, 0, n)

#define NTEST_N(n) \
  NTEST_0(n);      \
  NTEST_1(n);      \
  NTEST_2(n);      \
  NTEST_3(n);      \
  NTEST_4(n)

#ifndef TEST_F
# define TEST_F N
#endif

#define NTEST_CALL(n) PP_CAT(NTEST_, TEST_F)(n)


#ifdef TEST_N1
NTEST_CALL(TEST_N1);
#endif
#ifdef TEST_N2
NTEST_CALL(TEST_N2);
#endif
#ifdef TEST_N3
NTEST_CALL(TEST_N3);
#endif
#ifdef TEST_N4
NTEST_CALL(TEST_N4);
#endif
#ifdef TEST_N5
NTEST_CALL(TEST_N5);
#endif
//END ntest


//BEGIN stest
template<class L, class R>
struct make_stest_line;

template<std::size_t... L, std::size_t... R>
struct make_stest_line<
  std::integer_sequence<std::size_t, L...>,
  std::integer_sequence<std::size_t, R...>
> {
  using type = NAMESPACE::mp_index_of<x,
    std::integral_constant<int, int(L)>...,
    x,
    std::integral_constant<int, int((R+sizeof...(L)+1))>...
  >;
};

template<class L>
struct make_stest;

template<std::size_t N, std::size_t i>
using stest_fixgcc8 = make_stest_line<
  std::make_index_sequence<i>,
  std::make_index_sequence<N-i-1>>;

template<std::size_t... Ints>
struct make_stest<std::integer_sequence<std::size_t, Ints...>>
{
  using type = std::integer_sequence<std::size_t,
    stest_fixgcc8<sizeof...(Ints), Ints>::type::value...>;
};

// mp_index_of<x, x, 1..n>
// mp_index_of<x, 0, x, 2..n>
// mp_index_of<x, 0, 1, x, 3..n>
// ...
// mp_index_of<x, 0..n>
#define STEST_N(n)                                      \
  [[maybe_unused]] inline std::make_index_sequence<(n)> \
    FROM make_stest<std::make_index_sequence<(n)>>::type{}


#ifdef TEST_S1
STEST_N(TEST_S1);
# endif
#ifdef TEST_S2
STEST_N(TEST_S2);
# endif
#ifdef TEST_S3
STEST_N(TEST_S3);
# endif
#ifdef TEST_S4
STEST_N(TEST_S4);
# endif
#ifdef TEST_S5
STEST_N(TEST_S5);
# endif
//END stest


//BEGIN ltest
template<std::size_t I, class L>
struct make_ltest_line;

template<std::size_t I, std::size_t... Ints>
struct make_ltest_line<I, std::integer_sequence<std::size_t, Ints...>>
{
  using type = NAMESPACE::mp_index_of<x,
    itox<Ints, std::integral_constant<int, int(I)>>...,
    x
  >;
};

template<std::size_t N, class L>
struct make_ltest;

template<std::size_t N, std::size_t... Ints>
struct make_ltest<N, std::integer_sequence<std::size_t, Ints...>>
{
  using type = std::integer_sequence<int,
    make_ltest_line<Ints, std::make_index_sequence<N>>::type::value...>;

  using result = std::integer_sequence<int, ((void)Ints, N)...>;
};

#ifndef TEST_LN
# define TEST_LN 30
#endif

// mp_index_of<x, 0{n}...>
// mp_index_of<x, 1{n}...>
// ...
// mp_index_of<x, m{n}...>
#define LTEST_N(n, m)                                                            \
  [[maybe_unused]] inline make_ltest<(n), std::make_index_sequence<(m)>>::result \
    FROM make_ltest<(n), std::make_index_sequence<(m)>>::type{}


#ifdef TEST_L1
LTEST_N(TEST_L1, TEST_LN);
#endif
#ifdef TEST_L2
LTEST_N(TEST_L2, TEST_LN);
#endif
#ifdef TEST_L3
LTEST_N(TEST_L3, TEST_LN);
#endif
#ifdef TEST_L4
LTEST_N(TEST_L4, TEST_LN);
#endif
#ifdef TEST_L5
LTEST_N(TEST_L5, TEST_LN);
#endif
//END ltest


//BEGIN sitest
template<std::size_t i, class L, class... Before>
struct make_sitest_line;

template<std::size_t i, std::size_t... Ints, class... Before>
struct make_sitest_line<i, std::integer_sequence<std::size_t, Ints...>, Before...>
{
  using type = NAMESPACE::mp_index_of<
    x, Before..., std::integral_constant<int, int(i+Ints)>..., x>;
};

template<std::size_t N, class L, class... Before>
struct make_sitest;

template<std::size_t N, std::size_t i, class... Before>
using sitest_fixgcc8 = make_sitest_line<
  i, std::make_index_sequence<(N-i)>, Before...>;

template<std::size_t N, std::size_t... Ints, class... Before>
struct make_sitest<N, std::integer_sequence<std::size_t, Ints...>, Before...>
{
  using type = std::integer_sequence<int,
    sitest_fixgcc8<N, Ints, Before...>::type::value...,
    NAMESPACE::mp_index_of<x>::value>;

  using result = std::integer_sequence<int, (N-Ints)..., -1>;
  using r0 = std::integer_sequence<int, ((void)Ints, 0)..., -1>;
};

// mp_index_of<x, 0..n, x>
// mp_index_of<x, 0..(n-1), x>
// mp_index_of<x, 0..(n-2), x>
// ...
// mp_index_of<x, x>
#define SITEST_N(n)                                                             \
  [[maybe_unused]] inline make_sitest<n, std::make_index_sequence<(n)>>::result \
    FROM make_sitest<n, std::make_index_sequence<(n)>>::type{}


#ifdef TEST_SI1
SITEST_N(TEST_SI1);
#endif
#ifdef TEST_SI2
SITEST_N(TEST_SI2);
#endif
#ifdef TEST_SI3
SITEST_N(TEST_SI3);
#endif
#ifdef TEST_SI4
SITEST_N(TEST_SI4);
#endif
#ifdef TEST_SI5
SITEST_N(TEST_SI5);
#endif
//END sitest


//BEGIN si0test
// mp_index_of<x, n..0, x>
// mp_index_of<x, (n-1)..0, x>
// mp_index_of<x, (n-2)..0, x>
// ...
// mp_index_of<x, x>
# define SI0TEST_N(n)                                                          \
  [[maybe_unused]] inline make_sitest<n, std::make_index_sequence<(n)>, x>::r0 \
    FROM make_sitest<n, std::make_index_sequence<(n)>, x>::type{}


#ifdef TEST_SI01
SI0TEST_N(TEST_SI01);
#endif
#ifdef TEST_SI02
SI0TEST_N(TEST_SI02);
#endif
#ifdef TEST_SI03
SI0TEST_N(TEST_SI03);
#endif
#ifdef TEST_SI04
SI0TEST_N(TEST_SI04);
#endif
#ifdef TEST_SI05
SI0TEST_N(TEST_SI05);
#endif
//END si0test


//BEGIN sdtest
template<class L, class... Before>
struct make_sdtest_line;

template<std::size_t... Ints, class... Before>
struct make_sdtest_line<std::integer_sequence<std::size_t, Ints...>, Before...>
{
  using type = NAMESPACE::mp_index_of<
    x, Before..., std::integral_constant<int, int(Ints)>..., x>;
};

template<std::size_t N, class L, class... Before>
struct make_sdtest;

template<std::size_t N, std::size_t i, class... Before>
using sdtest_fixgcc8 = make_sdtest_line<
  std::make_index_sequence<(N-i)>, Before...>;

template<std::size_t N, std::size_t... Ints, class... Before>
struct make_sdtest<N, std::integer_sequence<std::size_t, Ints...>, Before...>
{
  using type = std::integer_sequence<int,
    sdtest_fixgcc8<N, Ints, Before...>::type::value...,
    NAMESPACE::mp_index_of<x>::value>;

  using result = std::integer_sequence<int, (N-Ints)..., -1>;
  using r0 = std::integer_sequence<int, ((void)Ints, 0)..., -1>;
};

// mp_index_of<x, n..0, x>
// mp_index_of<x, n..1, x>
// mp_index_of<x, n..2, x>
// ...
// mp_index_of<x, x>
#define SDTEST_N(n)                                                             \
  [[maybe_unused]] inline make_sdtest<n, std::make_index_sequence<(n)>>::result \
    FROM make_sdtest<n, std::make_index_sequence<(n)>>::type{}


#ifdef TEST_SD1
SDTEST_N(TEST_SD1);
#endif
#ifdef TEST_SD2
SDTEST_N(TEST_SD2);
#endif
#ifdef TEST_SD3
SDTEST_N(TEST_SD3);
#endif
#ifdef TEST_SD4
SDTEST_N(TEST_SD4);
#endif
#ifdef TEST_SD5
SDTEST_N(TEST_SD5);
#endif
//END sdtest


//BEGIN sd0test
// mp_index_of<x, n..0, x>
// mp_index_of<x, (n-1)..0, x>
// mp_index_of<x, (n-2)..0, x>
// ...
// mp_index_of<x, x>
# define SD0TEST_N(n)                                                          \
  [[maybe_unused]] inline make_sdtest<n, std::make_index_sequence<(n)>, x>::r0 \
    FROM make_sdtest<n, std::make_index_sequence<(n)>, x>::type{}


#ifdef TEST_SD01
SD0TEST_N(TEST_SD01);
#endif
#ifdef TEST_SD02
SD0TEST_N(TEST_SD02);
#endif
#ifdef TEST_SD03
SD0TEST_N(TEST_SD03);
#endif
#ifdef TEST_SD04
SD0TEST_N(TEST_SD04);
#endif
#ifdef TEST_SD05
SD0TEST_N(TEST_SD05);
#endif
//END sd0test

#endif

int main()
{}
