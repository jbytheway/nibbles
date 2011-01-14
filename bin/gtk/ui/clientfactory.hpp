#ifndef NIBBLES_GTK__UI__CLIENTFACTORY_HPP
#define NIBBLES_GTK__UI__CLIENTFACTORY_HPP

namespace nibbles { namespace gtk { namespace ui {

struct ClientFactory {
  virtual client::Client::Ptr makeClient(std::string const&) = 0;
};

}}}

#endif // NIBBLES_GTK__UI__CLIENTFACTORY_HPP

