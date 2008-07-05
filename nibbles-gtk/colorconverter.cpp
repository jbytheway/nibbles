#include "colorconverter.hpp"

namespace nibbles { namespace gtk {

Gdk::Color ColorConverter::toGdkColor(const Color& color)
{
  Gdk::Color c;
  c.set_rgb(color.get<red>()<<8, color.get<green>()<<8, color.get<blue>()<<8);
  return c;
}

Color ColorConverter::toColor(const Gdk::Color& color)
{
  return Color(color.get_red()>>8, color.get_green()>>8, color.get_blue()>>8);
}

}}

