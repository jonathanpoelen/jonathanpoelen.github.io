#include "shapes1.hpp"


using draw_func_type = void(void const*, std::ostream&);

template<class T>
inline draw_func_type const* draw_func_ptr = [](void const* base, std::ostream& out) {
  static_cast<T const*>(base)->draw(out);
};


//BEGIN drawable
struct Drawable
{
  template<class T>
  Drawable(T& x)
  : obj(&x)
  , draw_func(draw_func_ptr<T>)
  {}

  void draw(std::ostream& out) const
  {
    draw_func(obj, out);
  }

private:
  void* obj;
  draw_func_type* draw_func;
};
//END drawable


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
