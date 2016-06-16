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
    virtual const char* what() const throw();

private:
    const char* m_what;
};

inline void ThrowError(int error) {
    if (error != 0) {
        throw Error(error);
    }
}
}

#endif
