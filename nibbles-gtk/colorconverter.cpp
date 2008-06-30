#include "colorconverter.hpp"

namespace nibbles { namespace gtk {

Gdk::Color ColorConverter::toGdkColor(const Color& color)
{
  Gdk::Color c;
  c.set_rgb(color.get<red>()<<8, color.get<green>()<<8, color.get<blue>()<<8);
  return c;
}

}}

