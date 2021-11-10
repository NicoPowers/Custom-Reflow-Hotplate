#include <LiquidCrystal.h>

/***************************************************
  This is an example for the Adafruit Thermocouple Sensor w/MAX31855K

  Designed specifically to work with the Adafruit Thermocouple Sensor
  ----> https://www.adafruit.com/products/269

  These displays use SPI to communicate, 3 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include <AutoPID.h>
//#include <SoftPWM.h>
#include "Adafruit_MAX31855.h"

// Default connection is using software SPI, but comment and uncomment one of
// the two examples below to switch between software SPI and hardware SPI:

// Example creating a thermocouple instance with software SPI on any three
// digital IO pins.
#define MAXDO   3
#define MAXCS   4
#define MAXCLK  5

#define SSR_PIN 2

//pid settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 200
#define MAX_ON_TIME 1
#define KP 1
#define KI 0.01
#define KD 0.0001

#define PREHEAT 50.0 // reach this termperature in 60 seconds
#define SOAK 70.0 // reach this temperature in 120 seconds 
#define REFLOW 90.0 // hold this temperature for 20 seconds

double temperature, setPoint, outputVal;


// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

//input/output variables passed by reference, so they are updated automatically
AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

void pwm_for_time(unsigned int onTime, unsigned int offTime, unsigned long duration, bool checkMaxTemp);


void setup() {
  Serial.begin(115200);

  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc


  //  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");

  pinMode(SSR_PIN, OUTPUT);
  
  // preheat
  pwm_for_time(500, 500, 80000, false);
  pwm_for_time(100, 900, 70000, false);
  // soak
  pwm_for_time(600, 400, 30000, false);
  pwm_for_time(150, 850, 60000, false);
  pwm_for_time(330, 670, 60000, false);
  // reflow
  pwm_for_time(550, 450, 38000, false);
  tone(10, 444, 500);
  pwm_for_time(0, 1000, 40000, false);
  tone(10, 444, 500);
  pwm_for_time(100, 900, 50000, false);
  // open door with screwdriver
  tone(10, 444, 500);
  // cooldown
}



void loop() {

  setPoint = 0.0;

  //  Serial.println("Reflow operation completed. Turning off SSR");

  while (true) {

    temperature = thermocouple.readCelsius();
    if (isnan(temperature)) {
      //        Serial.println("Something wrong with thermocouple!");
    } else {
      Serial.print(millis() / 1000.0);
      Serial.print(',');
      Serial.print(temperature);
      Serial.print('\n');
    }

    delay(1000);


  }

}

void pwm_for_time(unsigned int onTime, unsigned int offTime, unsigned long duration, bool checkMaxTemp) {

  unsigned long currentTime = millis();
  unsigned long endTime = currentTime + duration;
  while (millis() < endTime) {
    digitalWrite(SSR_PIN, HIGH);
    delay(onTime);
    digitalWrite(SSR_PIN, LOW);
    delay(offTime);
    temperature = thermocouple.readCelsius();
    if (checkMaxTemp) {
      if (temperature >= 160) {
        digitalWrite(SSR_PIN, LOW);
        return;
      }
    }
    if (isnan(temperature)) {
      //        Serial.println("Something wrong with thermocouple!");
    } else {
      Serial.print(millis() / 1000.0);
      Serial.print(',');
      Serial.print(temperature);
      Serial.print('\n');
    }
  }
  digitalWrite(SSR_PIN, LOW);
}
