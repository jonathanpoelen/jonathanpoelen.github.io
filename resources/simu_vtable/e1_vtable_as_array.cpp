#include "shapes1.hpp"

#include <array>


//BEGIN vtable_type
using drawable_vtable = std::array<void const*, 1>;
//END vtable_type


//BEGIN drawable_vtable_for
using draw_func_type = void(void const*, std::ostream&);

template<class T>
inline draw_func_type const* draw_func_ptr = [](void const* base, std::ostream& out) {
  static_cast<T const*>(base)->draw(out);
};

template<class T>
inline drawable_vtable drawable_vtable_for{
  &draw_func_ptr<T>
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
    (*static_cast<draw_func_type const* const*>(vtable[0]))(obj, out);
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
