#include "Server.h"
#include "UVWrap.h"
#include <iostream>
#include <vector>

int main() {
    try {
        uint16_t port = 31184;

        uv_loop_t loop;
        UV::ThrowError(uv_loop_init(&loop));

        Server server(&loop);
        server.Listen(port);
        std::cout << "Listening on port " << port << std::endl;

        uv_run(&loop, UV_RUN_DEFAULT);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "(unknown error)" << std::endl;
        return 1;
    }
}
