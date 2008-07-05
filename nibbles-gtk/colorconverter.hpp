#ifndef NIBBLES_GTK__COLORCONVERTER_HPP
#define NIBBLES_GTK__COLORCONVERTER_HPP

#include <gdkmm/color.h>

#include <nibbles/color.hpp>

namespace nibbles { namespace gtk {

struct ColorConverter {
  static Gdk::Color toGdkColor(const Color&);
  static Color toColor(const Gdk::Color&);
};

}}

#endif // NIBBLES_GTK__COLORCONVERTER_HPP

