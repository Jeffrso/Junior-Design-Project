// IR collision sensor.
// Background subtraction: read GPIO 7 with IR LED on, then with it off, and subtract.
// Room light is in both readings so it cancels.
#include "distance.h"

// Tuning number. Set it once: put the track wall STOP_CM in front of the sensor
// and copy the "reflection" value that the bench test prints.
const int   STOP_MV = 49;   // reflection (mV) with wall at STOP_CM
const float STOP_CM = 10.0;

const int PAIRS      = 8;     // LED on/off pairs per reading: 8 x 2 ms = 16 ms, about two 120 Hz flicker periods
const int SETTLE_US  = 1000;  // wait after switching LED so phototransistor settles
const int ADC_TOP_MV = 2800;  // ADC can't read above about 3.1 V, so stay a bit under it

static int  reflectMv = 0;    // LED on minus LED off = light bounced back by the obstacle
static int  roomMv    = 0;    // LED off = background room light
static bool obstacle  = false;

void irSensorSetup() {
  pinMode(IR_LED_PIN, OUTPUT);
  digitalWrite(IR_LED_PIN, LOW);
}

// Room light so bright there's no room left on the ADC to see the wall: can't see, so stop (fail safe).
static bool blinded() {
  return roomMv + STOP_MV > ADC_TOP_MV;
}

bool collisionUpdate() {
  long diffSum = 0, offSum = 0;
  for (int i = 0; i < PAIRS; i++) {
    digitalWrite(IR_LED_PIN, HIGH);
    delayMicroseconds(SETTLE_US);
    int on = analogReadMilliVolts(IR_SENSE_PIN);
    digitalWrite(IR_LED_PIN, LOW);
    delayMicroseconds(SETTLE_US);
    int off = analogReadMilliVolts(IR_SENSE_PIN);
    diffSum += on - off;
    offSum += off;
  }
  reflectMv = diffSum / PAIRS;
  roomMv = offSum / PAIRS;

  if (reflectMv > STOP_MV || blinded()) {
    obstacle = true; // closer than STOP_CM
  } else if (reflectMv < STOP_MV * 7 / 10) {
    obstacle = false; // clear again once about 20% farther away
  }
  return obstacle;
}

bool obstacleAhead() {
  return obstacle;
}

void collisionTestLoop() {
  collisionUpdate();

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint < 100) return;
  lastPrint = millis();

  // Distance estimate from 1/d^2 law: d = STOP_CM * sqrt(STOP_MV / reflection)
  float cm = reflectMv > 0 ? STOP_CM * sqrtf((float)STOP_MV / reflectMv) : 99;
  Serial.printf("room light %4d mV | reflection %4d mV | ~%4.1f cm | %s\n", roomMv, reflectMv,
                cm > 99 ? 99 : cm, blinded() ? "TOO BRIGHT: STOP" : obstacle ? "OBSTACLE: STOP" : "clear");
}