#include "UVWrap.h"

namespace UV {

Error::Error(int number) : m_what(uv_strerror(number)) {
}

Error::~Error() throw() {
}

const char* Error::what() const throw() {
    return m_what;
}
}
