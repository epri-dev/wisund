#include <iostream>
#include <string>
#include <asio.hpp>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "Usage: demotest demo_ip\n"
            "where demo_ip may be a resolvable machine name or IPv6 address\n";
        return 1;
    }
    std::string distant_end{argv[1]};
    using asio::ip::udp;
    constexpr std::size_t max_length{1024};

    try {
        asio::io_service io_service;
        udp::socket socket(io_service, udp::endpoint(udp::v6(), 0));
        udp::resolver resolver(io_service);
        udp::endpoint endpoint = *resolver.resolve({udp::v6(), distant_end, "4321"});
        std::cout << "Enter message: ";
        char request[max_length];
        std::cin.getline(request, max_length);
        size_t request_len = std::strlen(request);
        socket.send_to(asio::buffer(request, request_len), endpoint);
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
    }
}
