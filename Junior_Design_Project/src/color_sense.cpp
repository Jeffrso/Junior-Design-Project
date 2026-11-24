// Changes for Lane Following:
//   - all 3 sensors are read at the same time, so one full read is ~6 ms
//   - room light is read right before each LED flash and subtracted (LED on - LED off)
//   - each sensor has its own reference values, set with COLOR_TEST_MODE
#include "color_sense.h"
#define R_BITS 12
#define MAX_VOLTS 3.3
#define SCALE_TO_FIVE 1.52

const int SAMPLES = 4; // ADC reads averaged per measurement
const int SETTLE_US = 500; // wait after switching an LED before reading
const float SCALE = 5.0 / 4095.0; // same units as our first reference values (ADC reading / 4095 * 5)

// Reference {R, G, B} for each color, for each sensor.
static float CAL[4][3] = {
        {0.4020, 0.1560, 0.3600}, // red
        {0.3057, 0.8271, 1.9550}, // blue
        {1.2367, 1.5333, 1.1333}, // yellow
        {0.0767, 0.1183, 0.2600}  // black
};

// Average SAMPLES readings of all 3 sensors
static void readSensors(float out[3]) {
    float sum[3] = {0, 0, 0};
    for (int i = 0; i < SAMPLES; i++) {
        for (int s = 0; s < 3; s++) {
            uint16_t raw_value = analogRead(COLOR_PINS[s]);
            sum[s] +=( (float)raw_value/ ((1 << R_BITS) - 1)) * (float)MAX_VOLTS * (float)SCALE_TO_FIVE;
        }
    }
    for (int s = 0; s < 3; s++) {
        out[s] = sum[s] / SAMPLES;
    }
}

// values[s] = {R, G, B} for sensor s, each = LED on - LED off (room light cancels)
static void readValues(float values[3][3]) {
    for (int c = 0; c < 3; c++) {
        float off[3], on[3];
        readSensors(off); // LEDs off: room light only
        for (int s = 0; s < 3; s++) digitalWrite(LED_PINS[s][c], HIGH); // all 3 sensors' LEDs of this color on
        delay(SETTLE_US);
        readSensors(on); // room light + LED reflection
        for (int s = 0; s < 3; s++) digitalWrite(LED_PINS[s][c], LOW);
        // delayMicroseconds(SETTLE_US); // let it go dark before next color
        for (int s = 0; s < 3; s++) {
            values[s][c] = on[s];
            // fabsf(on[s] - off[s]); // abs: works whichever way sensor is wired
        }
        delay(500);
    }
}

// Closest reference color for sensor s
static Colors classify(const float v[3]) {
    Colors best = BLACK_t;
    float bestDist = 1e9;
    const char* colors[3] = {"r", "g", "b"};
    for (int c = 0; c < 3; c++) {
        Serial.print(colors[c]);
        Serial.print(" = ");
        Serial.print(v[c]);
    }

    for (int c = 0; c < 4; c++) {
        float dist = 0;
        for (int k = 0; k < 3; k++) {
            float diff = v[k] - CAL[c][k];
            dist += diff * diff;
        }
        if (dist < bestDist) {
            bestDist = dist;
            best = (Colors)c;
        }
    }
    return best;
}

void colorSetup() {
//     uint8_t IR_LED_PIN = 4; // IR_IN_1 (D2)
//     uint8_t IR_LED_PIN_2 = 5; // IR_IN_2 (D3)
//     uint8_t IR_SENSE_PIN = 6;

// pinMode(LED_PINS[s][c], OUTPUT)
// pinMode(LED_PINS[s][c], OUTPUT)
// pinMode(LED_PINS[s][c], OUTPUT)


    for (int s = 0; s < 3; s++) {
        for (int c = 0; c < 3; c++) {
            pinMode(LED_PINS[s][c], OUTPUT);
            digitalWrite(LED_PINS[s][c], LOW);
        }
    }
    //init sensors to input
    for (int c = 0; c < 3; c++) {
        pinMode(COLOR_PINS[c], INPUT);
    }
    // analogReadResolution(12);
}

void colorReadAll(Colors colors[3]) {
    float v[3][3];
    readValues(v);
    const char* sensorNames[3] = {"Left", "Middle", "Right"};

    for (int s = 0; s < 3; s++) {
        Serial.print(sensorNames[s]);
        Serial.print(": ");
        colors[s] = classify(v[s]);
        Serial.println("");
    }
}

const char* colorName(Colors c) {
    switch (c) {
        case RED_t: return "red";
        case BLUE_t: return "blue";
        case YELLOW_t: return "yellow";
        case BLACK_t: return "black";
        default: return "none";
    }
}

// COLOR_TEST_MODE
// Prints each sensor's {R, G, B} and color 4 times a second. Built-in LED shows the middle sensor's color.
// Calibrate: put ONE sensor over a color, type 2 letters + Enter:
//   sensor l / m / r, then color r (red), b (blue), y (yellow), k (black). Example: my = middle sensor on yellow
//   p + Enter prints the entire table to paste over CAL above (if needed).
static void printTable() {
    const char* sensorNames[3] = {"Left", "Middle", "Right"};
    const char* colorNames[4] = {"red", "blue", "yellow", "black"};
    Serial.println("\n// Paste this over the CAL table in color_sense.cpp:");
    Serial.println("static float CAL[3][4][3] = {");
    for (int s = 0; s < 3; s++) {
        Serial.printf("    { // %s sensor\n", sensorNames[s]);
        for (int c = 0; c < 4; c++) {
            Serial.printf("        {%.4f, %.4f, %.4f}%s // %s\n",
                          CAL[c][0], CAL[c][1], CAL[c][2], c < 3 ? "," : " ", colorNames[c]);
        }
        Serial.printf("    }%s\n", s < 2 ? "," : "");
    }
    Serial.println("};");
}

// Average 20 reads of sensor s and save them as its reference for color c
// static void calibrate(int s, int c) {
//     const char* sensorNames[3] = {"Left", "Middle", "Right"};
//     float sum[3] = {0, 0, 0};
//     for (int i = 0; i < 20; i++) {
//         float v[3][3];
//         readValues(v);
//         for (int k = 0; k < 3; k++) {
//             sum[k] += v[s][k];
//         }
//     }
//     for (int k = 0; k < 3; k++) {
//         CAL[c][k] = sum[k] / 20;
//     }
//     Serial.printf("saved: %s sensor on %s = {%.4f, %.4f, %.4f}\n", sensorNames[s], colorName((Colors)c),
//                   CAL[c][0], CAL[c][1], CAL[c][2]);
// }

// static void handleCommand(String cmd) {
//     cmd.trim();
//     cmd.toLowerCase();
//     if (cmd == "p") {
//         printTable();
//         return;
//     }
//     int s = String("lmr").indexOf(cmd.charAt(0));
//     int c = String("rbyk").indexOf(cmd.charAt(1)); // same order as Colors: red, blue, yellow, black
//     if (cmd.length() != 2 || s < 0 || c < 0) {
//         Serial.println("type l/m/r then r/b/y/k (example: my = middle sensor on yellow), or p to print the table");
//         return;
//     }
//     calibrate(s, c);
// }

// void colorTestLoop() {
//     static String line = "";
//     while (Serial.available()) {
//         char ch = Serial.read();
//         if (ch == '\n') {
//             handleCommand(line);
//             line = "";
//         } else if (ch != '\r') {
//             line += ch;
//         }
//     }

//     static unsigned long lastPrint = 0;
//     if (millis() - lastPrint < 250) return;
//     lastPrint = millis();

//     float v[3][3];
//     readValues(v);
//     Colors col[3];
//     for (int s = 0; s < 3; s++) {
//         col[s] = classify(s, v[s]);
//     }
//     Serial.printf("L %.2f %.2f %.2f %-6s | M %.2f %.2f %.2f %-6s | R %.2f %.2f %.2f %-6s\n",
//                   v[0][0], v[0][1], v[0][2], colorName(col[0]),
//                   v[1][0], v[1][1], v[1][2], colorName(col[1]),
//                   v[2][0], v[2][1], v[2][2], colorName(col[2]));
//     int offM = analogReadMilliVolts(COLOR_PINS[1]);
//     for (int s = 0; s < 3; s++) for (int c = 0; c < 3; c++) digitalWrite(LED_PINS[s][c], HIGH); // all LEDs on (white)
//     delay(4);
//     int onM = analogReadMilliVolts(COLOR_PINS[1]);
//     for (int s = 0; s < 3; s++) for (int c = 0; c < 3; c++) digitalWrite(LED_PINS[s][c], LOW);
//     Serial.printf("M raw mV: LEDs off %d | LEDs on %d\n", offM, onM);
// }

