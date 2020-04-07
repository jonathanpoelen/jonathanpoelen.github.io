#include <iterator>

// juste pour simplifier l'écriture des fonctions
#define DECLTYPE_AUTO_RETURN_NOEXCEPT(...)                 \
  noexcept(noexcept(__VA_ARGS__)) -> decltype(__VA_ARGS__) \
  {                                                        \
    return (__VA_ARGS__);                                  \
  }

namespace adl_barrier
{
  // pour que front_impl() voit une fonction front() qu'elle pourrait utiliser
  struct na {};
  void front(na);

  // pour que begin(cont) utilise le begin de son namespace (ADL) ou std::begin()
  using std::begin;

  template<class Cont>
  constexpr auto front_impl(Cont& cont, int, int)
  DECLTYPE_AUTO_RETURN_NOEXCEPT(front(cont))

  template<class Cont>
  constexpr auto front_impl(Cont& cont, int, char)
  DECLTYPE_AUTO_RETURN_NOEXCEPT(cont.front())

  template<class Cont>
  constexpr auto front_impl(Cont& cont, char, char)
  DECLTYPE_AUTO_RETURN_NOEXCEPT(*begin(cont))
}

template<class Cont>
constexpr auto front(Cont&& cont)
// le perfect forwarding n'est pas respecté pour simplifier l'exemple
DECLTYPE_AUTO_RETURN_NOEXCEPT(adl_barrier::front_impl(cont, 1, 1))


namespace mylib
{
  struct A
  {
    char const * s;
  };

  constexpr char front(A const& a)
  {
    return a.s[0];
  }


  struct B
  {
    char const * s;
  };

  constexpr char const* begin(B const& b)
  {
    return b.s;
  }


  struct C
  {
    char const * s;

    constexpr char front() const
    {
      return s[0];
    }
  };
}

static_assert(front(mylib::A{"ax"}) == 'a'); // mylib::front(A)
static_assert(front(mylib::B{"bx"}) == 'b'); // *mylib::begin(B)
static_assert(front(mylib::C{"cx"}) == 'c'); // C::front()
