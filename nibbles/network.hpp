#ifndef NIBBLES__NETWORK_HPP
#define NIBBLES__NETWORK_HPP

namespace nibbles {

struct Network {
  typedef uint8_t PacketLength;
  static const uint16_t defaultPort = 53788;
  static const size_t maxPacketLen = 255;
};

}

#endif // NIBBLES__NETWORK_HPP

