#ifndef NIBBLES_GTK__UI_HPP
#define NIBBLES_GTK__UI_HPP

#include <boost/utility.hpp>

#include "options.hpp"

namespace nibbles { namespace gtk {

class UI : boost::noncopyable {
  public:
    UI(const Options&);
};

}}

#endif // NIBBLES_GTK__UI_HPP

