#ifndef NIBBLES__CLIENTID_HPP
#define NIBBLES__CLIENTID_HPP

#include <nibbles/utility/idbase.hpp>

namespace nibbles {

class ClientId : public utility::IdBase<uint32_t, ClientId> {
};

}

#endif // NIBBLES__CLIENTID_HPP

