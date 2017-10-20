#include <iostream>
#include <string>
#include <asio.hpp>

#ifndef SPEECH
#define SPEECH 0
#endif // SPEECH

#if SPEECH
#include <festival.h>

class Speaker {
public:
    Speaker(int heap_size = 610000, bool load_init_files = true);
    void say(const char *text);
};

Speaker::Speaker(int heap_size, bool load_init_files)
{
    festival_initialize(load_init_files ? 1 : 0,heap_size);
    // festival_eval_command("(voice_ked_diphone)");
}

void Speaker::say(const char *text) 
{
    festival_say_text("hello world");
}
#else
class Speaker {
public:
    Speaker() {}
    void say(const char *text) { std::cout << "SAY \"" << text << "\"\n"; }
};
#endif

int main()
{
    using asio::ip::udp;

    Speaker me;
    asio::io_service io_service;
    udp::socket socket(io_service, udp::endpoint(udp::v6(), 4321));
    constexpr std::size_t buflen{1024};
    for (;;) {
        udp::endpoint remote_endpoint;
        char data[buflen];
        std::size_t len = socket.receive_from(asio::buffer(data, buflen), remote_endpoint);
        data[len] = '\0';  // terminate string
        std::cout << std::string{data, len} << '\n';
        switch(data[0]) {
            case 'O':  // outage
                {
                    std::stringstream ss;
                    ss << "Power outage at Mrs. Bunny's house at " << &data[2];
                    me.say(ss.str().c_str());
                }
                break;
            case 'R':  // restoration
                {
                    std::stringstream ss;
                    ss << "Power was restored at Mrs. Bunny's house at " << &data[2];
                    me.say(ss.str().c_str());
                }
                break;
            default:
                // do nothing
                std::cout << "Unknown message type\n";
        }
    }
}
