#include "sevenSeg.h"

const uint8_t SEG_PINS[7] = {6, 16, 5, 42, 41, 2, 39};

void sevenseg_setup() {
    for (int i = 0; i < 7; i++) {
        pinMode(SEG_PINS[i], OUTPUT);
        digitalWrite(SEG_PINS[i], SEG_ACTIVE_LOW ? HIGH : LOW); // all segments off
    }
}

uint8_t sevenseg_decode(uint8_t number) {
    // 1 = segment on
    static const uint8_t lut[10] = {
        0b1111110, // 0
        0b0110000, // 1
        0b1101101, // 2
        0b1111001, // 3
        0b0110011, // 4
        0b1011011, // 5
        0b1011111, // 6
        0b1110000, // 7
        0b1111111, // 8
        0b1111011  // 9
    };

    if (number <= 9) {
        return lut[number];
    }
    return 0b0000000; // Default (all off)
}
