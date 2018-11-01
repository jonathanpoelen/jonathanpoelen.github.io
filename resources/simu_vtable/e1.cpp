#include "shapes1.hpp"

#include <tuple>


//BEGIN vtable_type
using draw_func_type = void(void const*, std::ostream&);

using drawable_vtable = std::tuple<draw_func_type const*>;
//END vtable_type


//BEGIN drawable_vtable_for
template<class T>
inline auto draw_func_ptr = [](void const* base, std::ostream& out) {
  static_cast<T const*>(base)->draw(out);
};

template<class T>
inline drawable_vtable drawable_vtable_for{
  draw_func_ptr<T>
};
//END drawable_vtable_for


//BEGIN drawable
struct Drawable
{
  template<class T>
  Drawable(T& x)
  : obj(&x)
  , vtable(drawable_vtable_for<T>)
  {}

  void draw(std::ostream& out) const
  {
    std::get<0>(vtable)(obj, out);
  }

private:
  void* obj;
  drawable_vtable const& vtable;
};
//END drawable


//BEGIN main
#include <iostream>

void draw(Drawable drawable, std::ostream& out)
{
  drawable.draw(out);
}

int main()
{
  Rect rect;
  Circle circle;

  draw(rect, std::cout); // affiche Rect
  draw(circle, std::cout); // affiche Circle
}
//END main
