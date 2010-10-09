#ifndef NIBBLES__NETWORK_HPP
#define NIBBLES__NETWORK_HPP

#include <cstdint>
#include <cstddef>

namespace nibbles {

struct Network {
  typedef std::uint8_t PacketLength;
  static const std::uint16_t defaultPort = 53788;
  static const std::size_t maxPacketLen = 255;
};

}

#endif // NIBBLES__NETWORK_HPP

