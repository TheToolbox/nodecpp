#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory>
#include <uv.h>

namespace uv {
    class EventLoop {
        uv_loop_t *loop;
        public:
        EventLoop() {
            loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
            uv_loop_init(loop);
        }
        ~EventLoop() {
            uv_loop_close(loop);
            free(loop);
        }
        void run() {
            uv_run(loop, UV_RUN_DEFAULT);
        }
    };

    class IPv4Address {
        public:
        int error = 0;
        struct sockaddr_in addr;

        IPv4Address(const char * address, const int port) {
            error = uv_ip4_addr(address, port, &addr);
        }
         
    };

    class TCPServer {
        private:
        struct sockaddr_in addr;
        uv_tcp_t * tcp_server;

        public:
        int error = 0;

        TCPServer() {
            tcp_server = (uv_tcp_t*) malloc(sizeof(*tcp_server));
            error = uv_tcp_init(uv_default_loop(), tcp_server);
        }

        int listen(const char * address, int &port, void(*on_connection)(uv_stream_t*, int)) {
            error = uv_ip4_addr(address, port, &addr);
            if (error) {return error;}
            error = uv_tcp_bind(tcp_server, (const struct sockaddr*)&addr, 0);
            if (error) {return error;}
            //figure out dynamic way to get 128
            r = uv_listen((uv_stream_t*)tcp_server, 128, on_connection);
            return error;
        }
    };
}

static void on_connection(uv_stream_t* server, int status) {
  uv_tcp_t* stream;
  int r;

  assert(status == 0);

  stream = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  assert(stream != NULL);

  r = uv_tcp_init(uv_default_loop(), stream);
  assert(r == 0);

  stream->data = server;

  r = uv_accept(server, (uv_stream_t*)stream);
  assert(r == 0);

  r = uv_read_start((uv_stream_t*)stream, alloc_cb, after_read);
  assert(r == 0);
}

int main() {
    auto loopy = uv::EventLoop();
    loopy.run();


    printf("Now quitting.\n");

    return 0;
}