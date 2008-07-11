#ifndef NIBBLES__DESERIALIZATIONEXCEPTION_HPP
#define NIBBLES__DESERIALIZATIONEXCEPTION_HPP

namespace nibbles {

class DeserializationException : std::runtime_error {
  public:
    DeserializationException(const std::string& message) :
      std::runtime_error(message)
    {}
};

}

#endif // NIBBLES__DESERIALIZATIONEXCEPTION_HPP

