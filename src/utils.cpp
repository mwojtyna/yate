#include "utils.hpp"
#include <cctype>
#include <cstdint>
#include <cstdio>

// https://stackoverflow.com/a/29865/9854703
void hexdump(void* ptr, size_t len) {
    uint8_t* buf = (uint8_t*)ptr;
    size_t i, j;
    printf("len: %zu\n", len);
    for (i = 0; i < len; i += 16) {
        printf("%06zx: ", i);

        for (j = 0; j < 16; j++) {
            if (i + j < len) {
                printf("%02x ", buf[i + j]);
            } else {
                printf("   ");
            }
        }
        printf(" ");

        for (j = 0; j < 16; j++) {
            if (i + j < len) {
                printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
            }
        }
        printf("\n");
    }
}
