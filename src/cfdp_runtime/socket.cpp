#include <cfdp_core/utils.hpp>
#include <cfdp_runtime/socket.hpp>

#include <bit>
#include <stdexcept>

namespace
{
constexpr int ip_addr_converted = 1;
constexpr int socket_error      = -1;
} // namespace

cfdp::runtime::sockets::Socket::Socket(SocketType sType, uint16_t port, const std::string& ipAddr)
    : handle(socket(AF_INET, utils::toUnderlying(sType), 0))
{
    if (handle == socket_error)
    {
        throw std::runtime_error{"Could not create a socket."};
    }

    addr.sin_family = AF_INET;
    addr.sin_port   = port;

    auto result = inet_pton(AF_INET, ipAddr.c_str(), &addr.sin_addr);

    if (result != ip_addr_converted)
    {
        throw std::runtime_error{"Could not parse passed IP address."};
    }
}

void cfdp::runtime::sockets::UDPClient::sendMessage(std::vector<uint8_t> message)
{
    auto length = message.size();
    auto addr   = std::bit_cast<sockaddr*>(&socket.addr);

    auto result = sendto(socket.handle, message.data(), length, 0, addr, sizeof(socket.addr));

    if (result == socket_error)
    {
        throw std::runtime_error{"Could not send data via socket."};
    }
}

void cfdp::runtime::sockets::UDPServer::socketBind()
{
    auto addr   = std::bit_cast<sockaddr*>(&socket.addr);
    auto result = bind(socket.handle, addr, sizeof(socket.addr));

    if (result == socket_error)
    {
        throw std::runtime_error{"Could not bind the socket."};
    }
}
