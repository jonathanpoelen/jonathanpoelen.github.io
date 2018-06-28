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
  template<class U, class... Us>
  union recursive_union
  {
    char dummy;
    U x;
    recursive_union<Us...> y;

    recursive_union() : dummy() {}
    ~recursive_union(){}
  };

  template<class U>
  union recursive_union<U>
  {
    char dummy;
    U x;

    recursive_union() : dummy() {}
    ~recursive_union(){}
  };

  template<class T, class... Us>
  T& get(recursive_union<T, Us...>& u)
  {
    return u.x;
  }

  template<class T, class U>
  T& get(U& u)
  {
    return get<T>(u.y);
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

  detail::recursive_union<Ts...> data;
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

// https://godbolt.org/g/xu9fwh
