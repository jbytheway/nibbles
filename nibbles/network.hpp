#ifndef NIBBLES__NETWORK_HPP
#define NIBBLES__NETWORK_HPP

#include <cstdint>
#include <cstddef>

namespace nibbles {

struct Network {
  typedef std::uint16_t PacketLength;
  static const std::uint16_t defaultPort = 53788; // "bbles" on calculator
  static const std::size_t maxPacketLen = 32768; // No particular reason
};

}

#endif // NIBBLES__NETWORK_HPP

