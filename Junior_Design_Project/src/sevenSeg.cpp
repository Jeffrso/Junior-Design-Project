
#include "sevenseg.h"

uint8_t sevenseg_decode(uint8_t number) {
    static const uint8_t lut[10] = {
        0b0000001, // 0
        0b1001111, // 1
        0b0010010, // 2
        0b0000110, // 3
        0b1001100, // 4
        0b0100100, // 5
        0b0100000, // 6
        0b0001111, // 7
        0b0000000, // 8
        0b0000100  // 9
    };

    if (number <= 9) {
        return lut[number];
    }
    return 0b1111111; // Default (all off for active-low)
}