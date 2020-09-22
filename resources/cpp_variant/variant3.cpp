#include <type_traits>

namespace detail
{
  template<class T, class... Ts>
  struct count_items_to_right_of;

  template<class T, class U, class... Us>
  struct count_items_to_right_of<T, U, Us...>
  : count_items_to_right_of<T, Us...>
  {};

  template<class T, class... Us>
  struct count_items_to_right_of<T, T, Us...>
  : std::integral_constant<std::size_t, sizeof...(Us)>
  {};
}

template<class T, class... Ts>
using mp_index_of = std::integral_constant<
  std::size_t,
  sizeof...(Ts) - detail::count_items_to_right_of<T, Ts...>::value - 1
>;


#include <memory>
#include <cassert>

namespace detail
{
  template<class U, class... Us>
  union VariadicUnion;

  class Uninit {};
}

//BEGIN in_place_index_t
// std::in_place_index_t
template<std::size_t i>
struct in_place_index_t
{
  explicit in_place_index_t() = default;
};
//END in_place_index_t


template<class... Ts>
class Variant
{
public:
  Variant() = default;
  Variant(Variant&&);
  Variant(Variant const&);

  template<class T>
  Variant(T&& x);

  ~Variant();

  Variant& operator=(Variant&&);
  Variant& operator=(Variant const&);

  template<class T>
  Variant& operator=(T&& x);

  template<class F>
  auto visit(F&&);

private:
  template<class T>
  static constexpr auto index_of = mp_index_of<std::decay_t<T>, Ts...>::value;

  template<class Variant2>
  void copy(Variant2&&);

  template<class Variant2>
  void init(Variant2&&);

  void destroy();


  detail::VariadicUnion<Ts..., detail::Uninit> union_;
  std::size_t type_index_ = sizeof...(Ts);
};

namespace detail
{
  template<class T, class... Ts>
  union VariadicUnion
  {
    char dummy;
    T value;
    VariadicUnion<Ts...> others;

    VariadicUnion() : dummy() {}
    ~VariadicUnion(){}

//BEGIN ctor_VariadicUnion
    template<class U>
    VariadicUnion(in_place_index_t<0>, U&& x)
    : value(std::forward<U>(x))
    {}

    template<std::size_t I, class U>
    VariadicUnion(in_place_index_t<I>, U&& x)
    : others(in_place_index_t<I-1>{}, std::forward<U>(x))
    {}
//END ctor_VariadicUnion
  };

  template<class T>
  union VariadicUnion<T>
  {
    char dummy;
    T value;

    VariadicUnion() : dummy() {}
    ~VariadicUnion(){}

    template<class U>
    VariadicUnion(in_place_index_t<0>, U&& x)
    : value(std::forward<U>(x))
    {}
  };

  template<class T, class... Ts>
  T& union_get(VariadicUnion<T, Ts...>& u)
  {
    return u.value;
  }

  template<class T, class... Ts>
  T& union_get(VariadicUnion<T, Ts...>&& u)
  {
    return u.value;
  }

  template<class T, class... Ts>
  T& union_get(VariadicUnion<T, Ts...> const& u)
  {
    return u.value;
  }

  template<class T, class... Ts>
  T& union_get(VariadicUnion<T, Ts...> const&& u)
  {
    return u.value;
  }

  template<class T, class U>
  T& union_get(U&& u)
  {
    return union_get<T>(u.others);
  }
}

//BEGIN impl
namespace detail
{
  template<std::size_t N, class U, class F>
  decltype(auto) visit_union(
    in_place_index_t<N>, U&& union_, std::size_t type_index, F&& f)
  {
    auto visit_impl = [&](auto rec, auto i, auto&& u){
      using Union = decltype(u);
      // VariadicUnion& u => decltype(u) => VariadicUnion&
      // VariadicUnion&& u => decltype(u) => VariadicUnion
      // static_cast<Union&&>(VariadicUnion&) -> VariadicUnion&
      // static_cast<Union&&>(VariadicUnion) -> VariadicUnion&&

      if constexpr (i.value != N-1u) {
        return (type_index == i.value)
          ? f(static_cast<Union&&>(u).value)
          : rec(rec, std::integral_constant<std::size_t, i.value+1>{},
                static_cast<Union&&>(u).others);
      }
      else {
        (void)rec;
        return f(static_cast<Union&&>(u).value);
      }
    };

    return visit_impl(
      visit_impl, std::integral_constant<std::size_t, 0>{},
      std::forward<U>(union_)
    );
  }
}

template<class... Ts>
Variant<Ts...>::Variant(Variant const& other)
{
  this->init(other);
}

template<class... Ts>
Variant<Ts...>::Variant(Variant&& other)
{
  this->move(std::move(other));
}

template<class... Ts>
template<class T>
Variant<Ts...>::Variant(T&& x)
: union_(in_place_index_t<index_of<T>>{}, std::forward<T>(x))
, type_index_(index_of<T>)
{}

template<class... Ts>
Variant<Ts...>& Variant<Ts...>::operator=(Variant const& other)
{
  if (other.type_index_ == type_index_) {
    this->copy(other);
  }
  else {
    this->destroy();
    this->init(other);
  }
  return *this;
}

template<class... Ts>
Variant<Ts...>& Variant<Ts...>::operator=(Variant&& other)
{
  if (other.type_index_ == type_index_) {
    this->copy(std::move(other));
  }
  else {
    this->destroy();
    this->init(std::move(other));
  }
  return *this;
}

template<class... Ts>
template<class T>
Variant<Ts...>& Variant<Ts...>::operator=(T&& x)
{
  if (index_of<T> == type_index_) {
    detail::union_get<std::decay_t<T>>(union_) = std::forward<T>(x);
  }
  else {
    this->destroy();
    ::new(&detail::union_get<std::decay_t<T>>(union_)) decltype(union_)(
      in_place_index_t<index_of<T>>{}, std::forward<T>(x)
    );
    type_index_ = index_of<T>;
  }
  return *this;
}

template<class... Ts>
Variant<Ts...>::~Variant()
{
  this->destroy();
}

template<class... Ts>
template<class Variant2>
void Variant<Ts...>::init(Variant2&& other)
{
  detail::visit_union(
    in_place_index_t<sizeof...(Ts)+1>{},
    std::forward<Variant2>(other).union_,
    other.type_index_,
    [this](auto&& x){
      using T = decltype(x);
      if constexpr (!std::is_same<detail::Uninit, T>::value) {
        ::new(&detail::union_get<std::decay_t<T>>(union_)) decltype(union_)(
          in_place_index_t<index_of<T>>{}, static_cast<T&&>(x)
        );
      }
    }
  );
  type_index_ = other.type_index_;
}

template<class... Ts>
template<class Variant2>
void Variant<Ts...>::copy(Variant2&& other)
{
  detail::visit_union(
    in_place_index_t<sizeof...(Ts)+1>{},
    std::forward<Variant2>(other).union_,
    other.type_index_,
    [this](auto&& x){
      using T = decltype(x);
      if constexpr (!std::is_same<detail::Uninit, T>::value) {
        detail::union_get<std::decay_t<T>>(union_) = static_cast<T&&>(x);
      }
    }
  );
  type_index_ = other.type_index_;
}

template<class... Ts>
void Variant<Ts...>::destroy()
{
  detail::visit_union(
    in_place_index_t<sizeof...(Ts)+1>{}, union_, type_index_,
    [this](auto&& x){
      using T = std::decay_t<decltype(x)>;
      if constexpr (!std::is_same<detail::Uninit, T>::value) {
        detail::union_get<T>(union_).~T();
        type_index_ = sizeof...(Ts);
      }
    }
  );
}

template<class... Ts>
template<class F>
auto Variant<Ts...>::visit(F&& f)
{
  assert(sizeof...(Ts) != type_index_);

  return detail::visit_union(
    in_place_index_t<sizeof...(Ts)>{}, union_, type_index_,
    std::forward<F>(f)
  );
}
//END impl

//BEGIN test
#include <iostream>

int main()
{
  using MyVariant = Variant<int, char const*>;
  MyVariant v;
  v = MyVariant{3};
  struct Visitor {
    void operator()(int x) { std::cout << "int: " << x << '\n'; }
    void operator()(char const* x) { std::cout << "char*: " << x << '\n'; }
   };
  v.visit(Visitor{});
  v = "plop";
  v.visit(Visitor{});
}
//END test
