#ifndef NIBBLES_SERVER__RETURNPATH_HPP
#define NIBBLES_SERVER__RETURNPATH_HPP

namespace nibbles { namespace server {

class ReturnPath {
  public:
    virtual ~ReturnPath() = 0;
};

inline ReturnPath::~ReturnPath() {}

}}

#endif // NIBBLES_SERVER__RETURNPATH_HPP

