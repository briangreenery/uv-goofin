#include "UVWrapper.h"
#include <iostream>

int main() {
    try {
        UV::Loop loop = UV::NewLoop();
        return 0;
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "(unknown error)" << std::endl;
        return 1;
    }
}
