#ifndef UVWrapper_h
#define UVWrapper_h

#include <exception>
#include <memory>
#include <uv.h>

namespace UV {

class Error : public std::exception {
public:
    explicit Error(int number);
    virtual ~Error() throw();
    virtual const char* what() throw();

private:
    const char* m_what;
};

inline void ThrowError(int error) {
    if (error != 0) {
        throw Error(error);
    }
}

struct DisposeLoop {
    void operator()(uv_loop_t*);
};

typedef std::unique_ptr<uv_loop_t, DisposeLoop> Loop;

Loop NewLoop();
}

#endif
