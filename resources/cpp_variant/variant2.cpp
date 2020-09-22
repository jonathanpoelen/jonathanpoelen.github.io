//BEGIN mp_index_of
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
//END mp_index_of


#include <memory>
#include <cassert>

namespace detail
{
  struct VariantBase
  {
    virtual std::unique_ptr<VariantBase> clone() = 0;
    virtual ~VariantBase() = default;
  };
}

template<class... Ts>
class Variant
{
public:
  Variant() = default;
  Variant(Variant&&) = default;
  Variant(Variant const&);

  template<class T>
  Variant(T&& x);

  Variant& operator=(Variant&&) = default;
  Variant& operator=(Variant const&);

  template<class T>
  Variant& operator=(T&& x);

  template<class F>
  auto visit(F&&);

private:
  std::unique_ptr<detail::VariantBase> impl_;
  std::size_t type_index_ = sizeof...(Ts);
};

namespace detail
{
  template<class T>
  struct VariantImpl : VariantBase
  {
    template<class U>
    VariantImpl(U&& x)
    : value_(std::forward<U>(x))
    {}

    std::unique_ptr<VariantBase> clone() override
    {
      return std::make_unique<VariantImpl>(value_);
    }

    T value_;
  };

  template<class T>
  auto make_variant_impl(T&& x)
  {
    return std::make_unique<VariantImpl<std::decay_t<T>>>(
      std::forward<T>(x));
  }
}

//BEGIN impl
template<class... Ts>
Variant<Ts...>::Variant(Variant const& other)
: impl_(other.impl_->clone())
, type_index_(other.type_index_) // ici
{}

template<class... Ts>
template<class T>
Variant<Ts...>::Variant(T&& x)
: impl_(detail::make_variant_impl(std::forward<T>(x)))
, type_index_(mp_index_of<std::decay_t<T>, Ts...>::value) // là
{}

template<class... Ts>
Variant<Ts...>& Variant<Ts...>::operator=(Variant const& other)
{
  impl_ = other.impl_ ? other.impl_->clone() : nullptr;
  type_index_ = other.type_index_; // ici aussi
  return *this;
}

template<class... Ts>
template<class T>
Variant<Ts...>& Variant<Ts...>::operator=(T&& x)
{
  impl_ = detail::make_variant_impl(std::forward<T>(x));
  type_index_ = mp_index_of<std::decay_t<T>, Ts...>::value; // et là
  return *this;
}
//END impl

//BEGIN visit
template<class... Ts>
template<class F>
auto Variant<Ts...>::visit(F&& f)
{
  assert(impl_);
  auto visit_impl = [&](auto rec, auto* t, auto*... ts){
    using T = std::decay_t<decltype(*t)>;
    using Impl = detail::VariantImpl<std::decay_t<T>>;
    if constexpr (sizeof...(ts)) {
      // plus de dynamic_cast, mais une comparaison d'entier + static_cast
      return type_index_ == mp_index_of<T, Ts...>::value
        ? f(static_cast<Impl*>(impl_.get())->value_)
        : rec(rec, ts...);
    }
    else {
      (void)rec;
      return f(static_cast<Impl*>(impl_.get())->value_);
    }
  };
  return visit_impl(visit_impl, static_cast<Ts*>(nullptr)...);
}
//END visit

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
