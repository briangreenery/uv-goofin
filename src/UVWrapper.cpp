#include "UVWrapper.h"

namespace UV {

Error::Error(int number) : m_what(uv_strerror(number)) {
}

Error::~Error() throw() {
}

const char* Error::what() throw() {
    return m_what;
}

Loop NewLoop() {
    std::unique_ptr<uv_loop_t> loop(new uv_loop_t);
    ThrowError(uv_loop_init(loop.get()));
    return Loop(loop.release());
}

void DisposeLoop::operator()(uv_loop_t* loop) {
    uv_loop_close(loop);
    delete loop;
}
}
