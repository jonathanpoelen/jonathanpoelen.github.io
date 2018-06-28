//BEGIN mp_index_of
#include <kvasir/mpl/mpl.hpp>

namespace mpl = kvasir::mpl;

template<class T, class... Ts>
using mp_index_of = mpl::uint_<
  sizeof...(Ts) -
  mpl::call<
    mpl::find_if<mpl::same_as<T>, mpl::size<>>,
    Ts...
  >::value
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
, type_index_(other.type_index_)
{}

template<class... Ts>
template<class T>
Variant<Ts...>::Variant(T&& x)
: impl_(detail::make_variant_impl(std::forward<T>(x)))
, type_index_(mp_index_of<std::decay_t<T>, Ts...>::value)
{}

template<class... Ts>
Variant<Ts...>& Variant<Ts...>::operator=(Variant const& other)
{
  impl_ = other.impl_ ? other.impl_->clone() : nullptr;
  type_index_ = other.type_index_;
  return *this;
}

template<class... Ts>
template<class T>
Variant<Ts...>& Variant<Ts...>::operator=(T&& x)
{
  impl_ = detail::make_variant_impl(std::forward<T>(x));
  type_index_ = mp_index_of<std::decay_t<T>, Ts...>::value;
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
  struct Visit{
    void operator()(int x) { std::cout << "int: " << x << '\n'; }
    void operator()(char const* x) { std::cout << "char*: " << x << '\n'; }
  };
  v.visit(Visit{});
  v = "plop";
  v.visit(Visit{});
}
//END test
