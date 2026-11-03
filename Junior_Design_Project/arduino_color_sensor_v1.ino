#define COLOR_SENSOR A0
#define R_BITS 10
#define MAX_VOLTS 5
#define RED_PIN 2
#define BLUE_PIN 3
#define GREEN_PIN 4
#define RED_OUT 6
#define BLUE_OUT 7
#define YELLOW_OUT 5
#define DEFAULT_SAMPLE 10
#define BATTERY_IN 11
#define min(a,b) (((a) < (b)) ? (a) : (b))


typedef enum {
        RED_t,
        BLUE_t,
        YELLOW_t, 
        BLACK_t,
} Colors;


const float RED_CAL[3] = {0.35, 0.17, 0.26};
const float BLUE_CAL[3] = {0.12, 0.83, 2.11};
const float YELLOW_CAL[3] = {0.95, 1.73, 0.77};
const float BLACK_CAL[3] = {0.06, 0.12, 0.22};
float brightness_offset = 0;

void setup() {
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_OUT, OUTPUT);
  pinMode(BLUE_OUT, OUTPUT);
  pinMode(YELLOW_OUT, OUTPUT);
  pinMode(COLOR_SENSOR, INPUT);
  pinMode(BATTERY_IN, INPUT);


}


float distance_squared(float *read_value, const float *ref_value) {
        float distance = 0;

        for (int i = 0; i < 3; i++) {
                float difference = (ref_value[i] - read_value[i]);
                distance += difference * difference;
        }

        return distance;
}


float average_color_read(uint8_t pin, uint8_t sample_size) {
      float sensor_val = 0;
      uint16_t raw_value = 0;
      float average_read = 0;

       for (int i = 0; i < sample_size; i++) {
          raw_value = analogRead(pin);
          sensor_val = ((float)raw_value / ((1 << R_BITS) - 1)) * MAX_VOLTS;
          average_read += sensor_val;
          delay(10);
      }

      average_read = average_read / sample_size;
      if (average_read < 0) {
        return 0;
      } 

      return average_read;
}


void calibrate_color_sensor(uint8_t pin) {
      brightness_offset = average_color_read(pin, DEFAULT_SAMPLE);
}


Colors read_color(uint8_t pin, uint8_t *rby_pins) {
        uint16_t raw_value = 0;
        Colors color;
        //values ordered (red, blue, yellow)
        char *ORDERING[3] = {"RED ", "GREEN ","BLUE "}; //used for debugging

        float voltage_values[3] = {0};
        for (int i = 0; i < 3; i++) {
                digitalWrite(rby_pins[i], HIGH);
                voltage_values[i] = average_color_read(pin, DEFAULT_SAMPLE) - brightness_offset;
                Serial.print(ORDERING[i]);
                Serial.println(voltage_values[i]);
                digitalWrite(rby_pins[i], LOW);
                delay(100);
        }
      

        float dr = distance_squared(voltage_values, RED_CAL);
        float db = distance_squared(voltage_values, BLUE_CAL);
        float dy = distance_squared(voltage_values, YELLOW_CAL);
        float dd = distance_squared(voltage_values, BLACK_CAL);

        float min_distance = min(dy, min(dd, min(dr, db)));

        if (dr == min_distance) return RED_t;
        else if (db == min_distance) return BLUE_t;
        else if (dy == min_distance) return YELLOW_t;
        else return BLACK_t;
}


void print_LED(Colors color) {
  if (color == RED_t) {
    digitalWrite(RED_OUT, HIGH);
    digitalWrite(YELLOW_OUT, LOW);
    digitalWrite(BLUE_OUT, LOW);
  } else if (color == BLUE_t) {
    digitalWrite(RED_OUT, LOW);
    digitalWrite(YELLOW_OUT, LOW);
    digitalWrite(BLUE_OUT, HIGH);
  } else if (color == YELLOW_t) {
    digitalWrite(RED_OUT, LOW);
    digitalWrite(YELLOW_OUT, HIGH);
    digitalWrite(BLUE_OUT, LOW);
  } else {
    digitalWrite(RED_OUT, LOW);
    digitalWrite(YELLOW_OUT, LOW);
    digitalWrite(BLUE_OUT, LOW);
  }
}


void loop() {

  float voltage_value = 0;
    uint8_t rby_pins[3] = {RED_PIN, GREEN_PIN, BLUE_PIN};
    calibrate_color_sensor(COLOR_SENSOR); 
  while(1) {


    
    // Serial.print("BRIGHTNESS:");
    // Serial.println(brightness_offset);
    Colors color = read_color(COLOR_SENSOR, rby_pins);
    // digitalWrite(BLUE_PIN, HIGH);
    // float value = average_color_read(COLOR_SENSOR, DEFAULT_SAMPLE);
    // Serial.println(value);
      Serial.print("DETECTED: ");
      if (color == RED_t) {
        Serial.println("RED");
      } else if (color == BLUE_t) {
         Serial.println("BLUE");
      } else if (color == YELLOW_t) {
          Serial.println("YELLOW");
      } else {
          Serial.println("BLACK");
      }

    delay(1000);






// battery testing stuff
  //   uint16_t battery_raw = analogRead(BATTERY_IN);
  //   float battery_volt_divide = ((float)battery_raw / ((1 << R_BITS) - 1)) * MAX_VOLTS;
  //   float batter_level = 1.6 * battery_volt_divide;
  //   Serial.println(battery_volt_divide);
  //   if (batter_level >= 3) {
  //     Serial.println("Good");
  //   } else if(batter_level >= 2.5) {
  //     Serial.println("Getting low");
  //   } else {
  //     Serial.println("LOW, please replace");
  //   delay(1000);
  // }

  }
}
