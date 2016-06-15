#include <iostream>
#include <uv.h>

int main() {
    uv_loop_t* loop = uv_loop_new();
    uv_loop_delete(loop);
    return 0;
}
