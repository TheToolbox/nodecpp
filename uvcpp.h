#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <functional>

namespace uv {
    #include <uv.h>

    typedef struct {
        uv_write_t req;
        uv_buf_t buf;
    } write_req_t;

    static void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
        buf->base = (char *)malloc(suggested_size);
        //assert(buf->base != NULL);
        //error handler later
        buf->len = suggested_size;
    }

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
        int run() {
            return uv_run(loop, UV_RUN_DEFAULT);
        }
        int runOnce(bool waitForPendingCallbacks) {
            if (waitForPendingCallbacks) {
                return uv_run(loop, UV_RUN_ONCE);
            } else {
                return uv_run(loop, UV_RUN_NOWAIT);
            }
        }
        bool isAlive() {
            return uv_loop_alive(loop) != 0;
        }
        void stop() {
            uv_stop(loop);
        }
        //not exposing uv_loop_size or uv_backend_fd or uv_backend_timeout
        uint64_t now() {
            return uv_now(loop);
        }
        void updateNow() {
            uv_update_time(loop);
        }
        void forEachHandle(void (*callback)(uv_handle_t* handle, void* arg)) {
            uv_walk(loop, callback, NULL);
        }
        uv_loop_t * getInternalLoop() {
            return loop;
        }
    };

    class Handle {
        private:
        uv_handle_t * handle;
        static void __onClose(uv_handle_t * handle) {
            //handle
        }
        public:
        Handle(uv_handle_type type) {

        }
        ~Handle() {
            //uv_close_handle(handle);
        }
        void onClose() {

        }
        bool isActive() {
            return 0 != uv_is_active(handle);
        }
        //don't think we need to expose ref/unref capabilities, seem entirely targeted to nodejs
        //not exposing due to possibility of memory leak/free error
        //getLoop() {
        //    return handle.loop
        //}
        //probably unnecessary
        //getType() {
        //
        //}

    };

    /*class IPv4Address {
        public:
        int error = 0;
        struct sockaddr_in addr;

        IPv4Address(const char * address, const int port) {
            error = uv_ip4_addr(address, port, &addr);
        }
         
    };*/

    class TCPServer {
        private:
        struct sockaddr_in addr;
        uv_tcp_t * tcp_server;
        uv_loop_t * loop;

        //TODO set up error callback
        static void onConnection(uv_stream_t* incoming, int status) {
            uv_tcp_t* stream;
            int error = 0;
            if (status != 0) {return;}
            stream = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
            if (error != 0) {return;}
            error = uv_tcp_init((uv_loop_t *)incoming->data, stream);
            if (error != 0) {return;}
            stream->data = incoming;
            error = uv_accept(incoming, (uv_stream_t*)stream);
            if (error != 0) {return;}
            error = uv_read_start((uv_stream_t*)stream, alloc_cb, onDataAvailable);
        }

        static void onDataAvailable(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
            if (nread <= 0 && buf->base != NULL) { free(buf->base); }
            if (nread == 0) { return; }
            if (nread < 0) {
                fprintf(stderr, "err: %s\n", uv_strerror(nread));
                uv_shutdown_t* req = (uv_shutdown_t*) malloc(sizeof(*req));
                if (req == NULL) { return; }
                int error = uv_shutdown(req, stream, onShutdown);
                return;
            }

            //dataCounter += nread;
            write_req_t * wr;
            wr = (write_req_t*) malloc(sizeof(*wr));
            if (wr == NULL) { return; }
            wr->buf = uv_buf_init(buf->base, nread);

            int error = uv_write(&wr->req, stream, &wr->buf, 1, onWrite);
        }

        static void onWrite(uv_write_t* req, int status) {
            write_req_t * wr = (write_req_t*)req;

            if (wr->buf.base != NULL) { free(wr->buf.base); }
            free(wr);

            if (status == 0) { return; }

            fprintf(stderr, "uv_write error: %s\n", uv_strerror(status));

            if (status == UV_ECANCELED) { return; }
            if (status != UV_EPIPE) { return; }
            uv_close((uv_handle_t*)req->handle, onClose);
        }

        static void onShutdown(uv_shutdown_t* req, int status) {
            /*assert(status == 0);*/
            if (status < 0) { fprintf(stderr, "err: %s\n", uv_strerror(status)); }
            //fprintf(stderr, "data received: %lu\n", data_cntr / 1024 / 1024);
            //data_cntr = 0;
            uv_close((uv_handle_t*)req->handle, onClose);
            free(req);
        }

        static void onClose(uv_handle_t* handle) { free(handle); }

        public:
        int error = 0;

        TCPServer(EventLoop& l){//uv_loop_t * eventLoop) {
            loop = l.getInternalLoop();
            tcp_server = (uv_tcp_t*) malloc(sizeof(*tcp_server));
            error = uv_tcp_init(loop, tcp_server);
        }

        int listen(const char * address, int port) {
            error = uv_ip4_addr(address, port, &addr);
            if (error) {return error;}
            error = uv_tcp_bind(tcp_server, (const struct sockaddr*)&addr, 0);
            if (error) {return error;}
            //figure out dynamic way to get 128
            tcp_server->data = loop;
            
            error = uv_listen((uv_stream_t*)tcp_server, 128, onConnection);
            return error;
        }
    };
}