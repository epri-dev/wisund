#define MG_DISABLE_DAV_AUTH 
#define MG_ENABLE_FAKE_DAVLOCK
extern "C" {
#include "mongoose.h"
}
#include <iostream>
#include <cstring>

class WebServer 
{
    const char *s_http_port;
    struct mg_mgr mgr;
    struct mg_connection *nc;
    struct mg_serve_http_opts s_http_server_opts;

    static void ev_handler(struct mg_connection *nc, int ev, void *p) {
        if (ev == MG_EV_HTTP_REQUEST) {
            mg_serve_http(nc, (struct http_message *) p, *(mg_serve_http_opts *)nc->user_data);
        }
    }

public:
    WebServer(const char *port = "8000") 
        : s_http_port{port}
    {
        std::memset(&s_http_server_opts, 0, sizeof(s_http_server_opts));
        mg_mgr_init(&mgr, NULL);
        nc = mg_bind(&mgr, s_http_port, ev_handler);

        // pass our options to event handler
        nc->user_data = &s_http_server_opts;

        // Set up HTTP server parameters
        mg_set_protocol_http_websocket(nc);
        s_http_server_opts.document_root = "../web_root";   // Serve web_root dir 
        s_http_server_opts.dav_document_root = ".";         // Allow access via WebDav
        s_http_server_opts.enable_directory_listing = "no";
    }
    void run() {
        printf("Starting web server on port %s\n", s_http_port);
        for (;;) {
            mg_mgr_poll(&mgr, 1000);
        }
    }
    ~WebServer() {
        mg_mgr_free(&mgr);
    }
};

int main(void) {
    WebServer server;
    server.run();
}
    
