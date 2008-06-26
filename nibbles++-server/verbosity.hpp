#ifndef NIBBLES_SERVER__VERBOSITY_HPP
#define NIBBLES_SERVER__VERBOSITY_HPP

namespace nibbles { namespace server {

class Verbosity {
  public:
    enum verbosity {
      debug,
      info,
      warning,
      error,
    };

    Verbosity() {};
    Verbosity(verbosity v) : value(v) {}
    operator verbosity() { return value; }
    friend bool operator<=(Verbosity l, Verbosity r) {
      return l.value <= r.value;
    }
  private:
    verbosity value;
};

}}

#endif // NIBBLES_SERVER__VERBOSITY_HPP

