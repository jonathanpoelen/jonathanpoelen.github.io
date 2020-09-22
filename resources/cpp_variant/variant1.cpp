#include <memory>

//BEGIN prototype
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
};
//END prototype


#include <cassert>

//BEGIN impl
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

template<class... Ts>
Variant<Ts...>::Variant(Variant const& other)
: impl_(other.impl_->clone())
{}

template<class... Ts>
template<class T>
Variant<Ts...>::Variant(T&& x)
: impl_(detail::make_variant_impl(std::forward<T>(x)))
{}

template<class... Ts>
Variant<Ts...>& Variant<Ts...>::operator=(Variant const& other)
{
  impl_ = other.impl_ ? other.impl_->clone() : nullptr;
  return *this;
}

template<class... Ts>
template<class T>
Variant<Ts...>& Variant<Ts...>::operator=(T&& x)
{
  impl_ = detail::make_variant_impl(std::forward<T>(x));
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
    using Impl = detail::VariantImpl<std::decay_t<decltype(*t)>>;
    if constexpr (sizeof...(ts)) {
      auto* impl = dynamic_cast<Impl*>(impl_.get());
      return impl ? f(impl->value_) : rec(rec, ts...);
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
