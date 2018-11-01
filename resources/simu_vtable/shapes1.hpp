#pragma once

#include <ostream>

//BEGIN shape
struct Rect
{
  void draw(std::ostream& out) const
  {
    out << "Rect\n";
  }
};

struct Circle
{
  void draw(std::ostream& out) const
  {
    out << "Circle\n";
  }
};
//END shape
