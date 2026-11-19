#include "color_sense.h"

const int SAMPLES = 4; // ADC reads averaged per measurement
const int LED_ON_MS = 15; // how long each color is on before reading (phototransistor needs a few ms)
const int LED_OFF_MS = 15; // dark time after each color, so next reading starts clean
const float SCALE = 5.0 / 4095.0; // same units as our first reference values (ADC reading / 4095 * 5)

// Reference {R, G, B} for each color, for each sensor.
static float CAL[3][4][3] = {
    { // Left sensor
        {9.0000, 0.0000, 0.0000}, // red
        {2.3375, 0.0000, 0.0000}, // blue
        {0.1116, 0.0000, 0.0000}, // yellow
        {0.0000, 0.0000, 0.0000}  // black
    },
    { // Middle sensor
        {0.5055, 0.0000, 0.0000}, // red
        {3.8829, 0.0000, 0.0000}, // blue
        {1.6316, 0.0000, 0.0000}, // yellow
        {0.1755, 0.0000, 0.0000}  // black
    },
    { // Right sensor
        {0.3142, 0.0000, 0.0000}, // red
        {3.7725, 0.0000, 0.0000}, // blue
        {1.9428, 0.0000, 0.0000}, // yellow
        {0.0000, 0.0000, 0.0000}  // black
    }
};

// Turn one LED color (c = 0 red, 1 green, 2 blue) on or off on all 3 sensors.
// pinMode is set here too, so pin is always an output when we flash it.
static void setColor(int c, uint8_t level) {
    for (int s = 0; s < 3; s++) {
        pinMode(LED_PINS[s][c], OUTPUT);
        digitalWrite(LED_PINS[s][c], level);
    }
}

// Average SAMPLES readings of all 3 sensors
static void readSensors(float out[3]) {
    long sum[3] = {0, 0, 0};
    for (int i = 0; i < SAMPLES; i++) {
        for (int s = 0; s < 3; s++) {
            sum[s] += analogRead(COLOR_PINS[s]);
        }
    }
    for (int s = 0; s < 3; s++) {
        out[s] = sum[s] * SCALE / SAMPLES;
    }
}

// One number per sensor: ALL LED colors on together ("blue flash" version), (LED on - LED off).
// Stored in values[s][0]; values[s][1] and [2] stay 0.
static void readValues(float values[3][3]) {
    float off[3], on[3];
    readSensors(off);                          // LEDs off: room light only
    for (int c = 0; c < 3; c++) setColor(c, HIGH); // every color on, all 3 sensors
    delay(LED_ON_MS);
    readSensors(on);                           // room light + LED reflection
    for (int c = 0; c < 3; c++) setColor(c, LOW);
    delay(LED_OFF_MS);
    for (int s = 0; s < 3; s++) {
        values[s][0] = fabsf(on[s] - off[s]);  // abs: works whichever way the sensor is wired
        values[s][1] = 0;
        values[s][2] = 0;
    }
}

// Closest reference color for sensor s
static Colors classify(int s, const float v[3]) {
    Colors best = BLACK_t;
    float bestDist = 1e9;
    for (int c = 0; c < 4; c++) {
        float dist = 0;
        for (int k = 0; k < 3; k++) {
            float diff = v[k] - CAL[s][c][k];
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
    for (int s = 0; s < 3; s++) {
        for (int c = 0; c < 3; c++) {
            pinMode(LED_PINS[s][c], OUTPUT); // all 9 LED pins are outputs, starting off
            digitalWrite(LED_PINS[s][c], LOW);
        }
    }
    analogReadResolution(12);
}

void colorReadAll(Colors colors[3]) {
    float v[3][3];
    readValues(v);
    for (int s = 0; s < 3; s++) {
        colors[s] = classify(s, v[s]);
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
// Prints each sensor's {R, G, B} and color once a second.
// Commands (type, then Enter):
//   t  = show each LED color for 2 s (red, then green, then blue) so we can see them
//   l / m / r + r / b / y / k = calibrate ONE sensor on a color. Example: my = middle sensor on yellow
//   p  = print the whole table to paste over CAL above
static void printTable() {
    const char* sensorNames[3] = {"Left", "Middle", "Right"};
    const char* colorNames[4] = {"red", "blue", "yellow", "black"};
    Serial.println("\n// Paste this over the CAL table in color_sense.cpp:");
    Serial.println("static float CAL[3][4][3] = {");
    for (int s = 0; s < 3; s++) {
        Serial.printf("    { // %s sensor\n", sensorNames[s]);
        for (int c = 0; c < 4; c++) {
            Serial.printf("        {%.4f, %.4f, %.4f}%s // %s\n",
                          CAL[s][c][0], CAL[s][c][1], CAL[s][c][2], c < 3 ? "," : " ", colorNames[c]);
        }
        Serial.printf("    }%s\n", s < 2 ? "," : "");
    }
    Serial.println("};");
}

// Average 20 reads of sensor s and save them as its reference for color c
static void calibrate(int s, int c) {
    const char* sensorNames[3] = {"Left", "Middle", "Right"};
    float sum[3] = {0, 0, 0};
    for (int i = 0; i < 20; i++) {
        float v[3][3];
        readValues(v);
        for (int k = 0; k < 3; k++) {
            sum[k] += v[s][k];
        }
    }
    for (int k = 0; k < 3; k++) {
        CAL[s][c][k] = sum[k] / 20;
    }
    Serial.printf("saved: %s sensor on %s = {%.4f, %.4f, %.4f}\n", sensorNames[s], colorName((Colors)c),
                  CAL[s][c][0], CAL[s][c][1], CAL[s][c][2]);
}

// Show each LED color for 2 s so we can check red, green, and blue all light up
static void showColors() {
    const char* names[3] = {"RED (pins 14, 17, 1)", "GREEN (pins 15, 18, 2)", "BLUE (pins 16, 21, 38)"};
    for (int c = 0; c < 3; c++) {
        Serial.printf("%s on: all 3 LEDs should show this color\n", names[c]);
        setColor(c, HIGH);
        delay(2000);
        setColor(c, LOW);
    }
    Serial.println("done");
}

static void handleCommand(String cmd) {
    cmd.trim();
    cmd.toLowerCase();
    if (cmd == "p") {
        printTable();
        return;
    }
    if (cmd == "t") {
        showColors();
        return;
    }
    int s = String("lmr").indexOf(cmd.charAt(0));
    int c = String("rbyk").indexOf(cmd.charAt(1)); // same order as Colors: red, blue, yellow, black
    if (cmd.length() != 2 || s < 0 || c < 0) {
        Serial.println("t = show LED colors | l/m/r then r/b/y/k = calibrate (example: my) | p = print table");
        return;
    }
    calibrate(s, c);
}

void colorTestLoop() {
    static String line = "";
    while (Serial.available()) {
        char ch = Serial.read();
        if (ch == '\n') {
            handleCommand(line);
            line = "";
        } else if (ch != '\r') {
            line += ch;
        }
    }

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint < 1000) return;
    lastPrint = millis();

    float v[3][3];
    readValues(v);
    Colors col[3];
    for (int s = 0; s < 3; s++) {
        col[s] = classify(s, v[s]);
    }
    Serial.printf("L %.2f %.2f %.2f %-6s | M %.2f %.2f %.2f %-6s | R %.2f %.2f %.2f %-6s\n",
                  v[0][0], v[0][1], v[0][2], colorName(col[0]),
                  v[1][0], v[1][1], v[1][2], colorName(col[1]),
                  v[2][0], v[2][1], v[2][2], colorName(col[2]));
}
