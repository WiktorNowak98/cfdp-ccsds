#pragma once

#include <exception>
#include <string>

namespace cfdp::exception
{
class CfdpException : public std::exception
{
  public:
    CfdpException(const char* message) : message(message) {}

    [[nodiscard]]
    inline const char* what() const noexcept override
    {
        return message.c_str();
    }

  private:
    std::string message;
};

class EncodeToBytesException : public CfdpException
{
  public:
    EncodeToBytesException(const char* message) : CfdpException(message) {}
};

class DecodeFromBytesException : public CfdpException
{
  public:
    DecodeFromBytesException(const char* message) : CfdpException(message) {}
};
} // namespace cfdp::exception
