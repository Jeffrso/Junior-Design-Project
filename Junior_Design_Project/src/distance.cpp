// IR collision sensor.
// 1. Each reading = LEDs on minus LEDs off. Room light is in both, so it cancels (works in any lighting)
// 2. At power-up: average 10 readings with nothing in front = baseline (LED light that leaks
//    straight into photodiode). After that: signal = reading - baseline
// 3. Signal above THRESHOLD_MV = obstacle. Reflection falls off as 1/d^2, so threshold sets stop distance
#include "distance.h"

const int THRESHOLD_MV = 4; // signal (mV) with the wall at stop distance
const int PAIRS = 8; // on/off pairs per reading (16 ms, averages out light flicker)
const int SETTLE_US = 1000; // let photodiode settle after switching LEDs

static int baselineMv = 0;
static int signalMv = 0;
static int roomMv = 0; // LEDs off = room light (printed)
static bool obstacle = false;
static int currentPeakMv = 0;
static int Avg_Samples = 15;

static void setLeds(uint8_t level) {
  digitalWrite(IR_LED_PIN, level);
  digitalWrite(IR_LED_PIN_2, level);
}

// One reading: average of (LEDs on - LEDs off)
static int readReflection() {
  long diffSum = 0, offSum = 0;
  for (int i = 0; i < PAIRS; i++) {
    setLeds(HIGH);
    delayMicroseconds(SETTLE_US);
    int on = analogReadMilliVolts(IR_SENSE_PIN);
    setLeds(LOW);
    delayMicroseconds(SETTLE_US);
    int off = analogReadMilliVolts(IR_SENSE_PIN);
    diffSum += on - off;
    offSum += off;
  }
  roomMv = offSum / PAIRS;
  return diffSum / PAIRS;
}

static int maxVoltage() {
  setLeds(HIGH);
  delayMicroseconds(SETTLE_US);
  int max = analogReadMilliVolts(IR_SENSE_PIN);
  setLeds(LOW);
  delayMicroseconds(SETTLE_US);
  return max;

}

static int getAveragedPeak(){
  long sum = 0;
  for (int i = 0; i < Avg_Samples; i++) {
    setLeds(HIGH);
    delayMicroseconds(SETTLE_US);
    sum += analogReadMilliVolts(IR_SENSE_PIN);
    setLeds(LOW);
    delayMicroseconds(SETTLE_US);
  }
  return sum / Avg_Samples;
}

bool collisionUpdate() {
  currentPeakMv = getAveragedPeak();
  //currentPeakMv = maxVoltage();
  if (currentPeakMv > (baselineMv + THRESHOLD_MV)) {
    obstacle = true;
  } else if (currentPeakMv < (baselineMv + (THRESHOLD_MV / 2))) {
    // prevent flickering at the edge??
    obstacle = false; 
  }
  return obstacle;
}

void irSensorSetup() {
  pinMode(IR_LED_PIN, OUTPUT);
  pinMode(IR_LED_PIN_2, OUTPUT);
  setLeds(LOW);

  long sum = 0; // baseline
  long currV = 0;

  for (int i = 0; i < 10; i++) {
    currV = maxVoltage();
    sum += maxVoltage();
    Serial.printf("Sample %d: %4d mV\n", i + 1, currV);
  }
  baselineMv = sum / 10;
    
  delay(50); 
  
}


// bool collisionUpdate() {
//   signalMv = readReflection() - baselineMv;
//   if (signalMv > THRESHOLD_MV + baselineMv) {
//     obstacle = true;
//   } else if (signalMv < THRESHOLD_MV / 2) {
//     obstacle = false; // clears a bit farther out, so it doesn't flicker at the edge
//   }
//   return obstacle;
// }

bool obstacleAhead() {
  return obstacle;
}

void collisionTestLoop() {
  collisionUpdate();

  static unsigned long lastPrint = 0;
  if (millis() - lastPrint < 100) return;
  lastPrint = millis();

  // Serial.printf("room light %4d mV | baseline %3d mV | signal %4d mV | %s\n",
  //               roomMv, baselineMv, signalMv, obstacle ? "OBSTACLE: STOP" : "clear");

  int livePeak = maxVoltage(); 
  int avgPeak = currentPeakMv;

  Serial.printf("Curr Peak: %4d mV | baseline %4d mV | avg %4d mV | %s\n",
                livePeak, baselineMv, avgPeak, obstacle ? "OBSTACLE: STOP" : "clear");

}
