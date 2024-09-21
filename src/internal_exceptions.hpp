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

class BytesEncodeException : public CfdpException
{
  public:
    BytesEncodeException(const char* message) : CfdpException(message) {}
};

class BytesDecodeException : public CfdpException
{
  public:
    BytesDecodeException(const char* message) : CfdpException(message) {}
};
} // namespace cfdp::exception
