#ifndef NIBBLES__PACKET_HPP
#define NIBBLES__PACKET_HPP

namespace nibbles {

class Packet {
  public:
    Packet(uint8_t const* const data, size_t const dataLen) :
      data_(data),
      dataLen_(dataLen)
    {}
  private:
    uint8_t const* const data_;
    size_t const dataLen_;
};

}

#endif // NIBBLES__PACKET_HPP

