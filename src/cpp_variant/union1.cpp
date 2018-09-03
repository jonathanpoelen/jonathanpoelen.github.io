#include <type_traits>

//BEGIN aligned_union
template<class... Ts>
struct AlignedStorage
{
  template<class T>
  T& get()
  {
    return *reinterpret_cast<T*>(&data);
  }

private:
  std::aligned_union_t<0, Ts...> data;
};
//END aligned_union

//BEGIN union
namespace detail
{
  template<class T, class... Ts>
  union RecursiveUnion
  {
    char dummy;
    T value;
    RecursiveUnion<Ts...> others;

    RecursiveUnion() : dummy() {}
    ~RecursiveUnion(){}
  };

  template<class T>
  union RecursiveUnion<T>
  {
    char dummy;
    T value;

    RecursiveUnion() : dummy() {}
    ~RecursiveUnion(){}
  };

  template<class T, class... Ts>
  T& get(RecursiveUnion<T, Ts...>& u)
  {
    return u.value;
  }

  template<class T, class U>
  T& get(U& u)
  {
    return get<T>(u.others);
  }
}

template<class... Ts>
struct UnionStorage
{
  template<class T>
  T& get()
  {
    return detail::get<T>(data);
  }

private:

  detail::RecursiveUnion<Ts...> data;
};
//END union

#include <iostream>

int main()
{
  using cstr_t = char const*;
  AlignedStorage<int, cstr_t> a;
  UnionStorage<int, cstr_t> b;

  new (&a.get<int>()) int{3};
  new (&b.get<int>()) int{3};

  std::cout << a.get<int>() << ' ' << b.get<int>() << std::endl;

  new (&a.get<cstr_t>()) cstr_t{"plop"};
  new (&b.get<cstr_t>()) cstr_t{"plop"};

  std::cout << a.get<cstr_t>() << ' ' << b.get<cstr_t>() << std::endl;
}
