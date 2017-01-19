#define MG_DISABLE_DAV_AUTH 
#define MG_ENABLE_FAKE_DAVLOCK
extern "C" {
#include "mongoose.h"
}
#include <iostream>
#include <string>
#include <cstring>
#include <atomic>
#include <thread>
#include <fstream>

static std::atomic_int done{false};

struct device_settings {
  char setting1[100];
  char setting2[100];
};

static std::fstream diag("moocow");

static struct device_settings s_settings{"value1", "value2"};

static void handle_save(struct mg_connection *nc, struct http_message *hm) {
  // Get form variables and store settings values
  mg_get_http_var(&hm->body, "setting1", s_settings.setting1,
                  sizeof(s_settings.setting1));
  mg_get_http_var(&hm->body, "setting2", s_settings.setting2,
                  sizeof(s_settings.setting2));

  // Send response
  mg_printf(nc, "%s", "HTTP/1.1 302 OK\r\nLocation: /\r\n\r\n");
}

static void handle_get_cpu_usage(struct mg_connection *nc) {
  // Generate random value, as an example of changing CPU usage
  // Getting real CPU usage depends on the OS.
  int cpu_usage = (double) rand() / RAND_MAX * 100.0;

  // Use chunked encoding in order to avoid calculating Content-Length
  mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

  // Output JSON object which holds CPU usage data
  mg_printf_http_chunk(nc, "{ \"result\": %d }", cpu_usage);

  // Send empty chunk, the end of response
  mg_send_http_chunk(nc, "", 0);
}

static void handle_ssi_call(struct mg_connection *nc, const char *param) {
  if (strcmp(param, "setting1") == 0) {
    mg_printf_html_escape(nc, "%s", s_settings.setting1);
  } else if (strcmp(param, "setting2") == 0) {
    mg_printf_html_escape(nc, "%s", s_settings.setting2);
  }
}

class WebServer 
{
    const char *s_http_port;
    struct mg_mgr mgr;
    struct mg_connection *nc;
    struct mg_serve_http_opts s_http_server_opts;

    static void ev_handler(mg_connection *nc, int ev, void *p) {
        http_message *hm = (http_message *)p;
        switch (ev) {
            case MG_EV_HTTP_REQUEST:
                if (mg_vcmp(&hm->uri, "/save") == 0) {
                    handle_save(nc, hm);
                } else if (mg_vcmp(&hm->uri, "/get_diag") == 0) {
                    handle_get_cpu_usage(nc);
                } else {
                    // serve static content 
                    mg_serve_http(nc, hm, *(mg_serve_http_opts *)nc->user_data);
                }
                break;
            case MG_EV_SSI_CALL:
                handle_ssi_call(nc, (const char *)p);
                break;
            default:
                break;
        }
    }

public:
    WebServer(const char *webroot, const char *port = "8000") 
        : s_http_port{port}
    {
        std::memset(&s_http_server_opts, 0, sizeof(s_http_server_opts));
        mg_mgr_init(&mgr, NULL);
        nc = mg_bind(&mgr, s_http_port, ev_handler);

        // pass our options to event handler
        nc->user_data = &s_http_server_opts;

        // Set up HTTP server parameters
        mg_set_protocol_http_websocket(nc);
        s_http_server_opts.document_root = webroot;   // Serve web_root dir 
        s_http_server_opts.dav_document_root = ".";         // Allow access via WebDav
        s_http_server_opts.enable_directory_listing = "no";
    }
    void run() {
        std::cout << "Starting web server on port " << s_http_port << "\n";
        while (!done) { 
            static time_t last_time;
            time_t now = time(NULL);
            mg_mgr_poll(&mgr, 1000);
            if (now - last_time > 0) {
                push_data_to_all_websocket_connections(&mgr);
                last_time = now;
            }
        }
    }

    static void push_data_to_all_websocket_connections(struct mg_mgr *m) {
        struct mg_connection *c;
        std::string line;
        std::getline(diag, line);
        std::cout << "Got: " << line << "\n";
        for (c = mg_next(m, NULL); c != NULL; c = mg_next(m, c)) {
            if (c->flags & MG_F_IS_WEBSOCKET) {
                mg_printf_websocket_frame(c, WEBSOCKET_OP_TEXT, "%s", line.c_str());
            }
        }
    }

    virtual ~WebServer() {
        mg_mgr_free(&mgr);
        diag.close();
    }
};

void serve(const char *webroot) {
    WebServer server(webroot);
    server.run();
}
    
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: web_server web_root_dir\n";
        return 1;
    }
    std::thread web{serve, argv[1]};
    std::string command;
    std::cout << "Enter the word \"quit\" to exit the program and shut down the server\n";
    while (!done && std::cin >> command) {
        if (command == "quit") {
            std::cout << "Shutting down web server now...\n";
            done = true;
        }
    }
    std::cout << "Joining web server thread...\n";
    web.join();
}
