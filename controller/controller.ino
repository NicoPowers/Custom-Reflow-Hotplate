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
#include <SoftPWM.h>
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
#define KP 0.5
#define KI 0.01
#define KD 0.0001

#define PREHEAT 23.0 // reach this termperature in 60 seconds
#define SOAK 120.0 // reach this temperature in 120 seconds 
#define REFLOW 140.0 // hold this temperature for 20 seconds

double temperature, setPoint, outputVal;


// initialize the Thermocouple
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

//input/output variables passed by reference, so they are updated automatically
AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

void goToTemp(double temp, int reachTime, int holdTime);

void setup() {
  Serial.begin(9600);

  while (!Serial) delay(1); // wait for Serial on Leonardo/Zero, etc


  Serial.println("MAX31855 test");
  // wait for MAX chip to stabilize
  delay(500);
  Serial.print("Initializing sensor...");
  if (!thermocouple.begin()) {
    Serial.println("ERROR.");
    while (1) delay(10);
  }
  Serial.println("DONE.");

  pinMode(SSR_PIN, OUTPUT);

  //if temperature is more than 4 degrees below or above setpoint, OUTPUT will be set to min or max respectively
  myPID.setBangBang(2);
  myPID.setTimeStep(1000);

  goToTemp(PREHEAT, 60, 100);
  Serial.println("DONE WITH PREHEAT!");
  delay(1000);
  goToTemp(SOAK, 10, 120);
  Serial.println("DONE WITH SOAK!");
  delay(1000);
  goToTemp(REFLOW, 10, 10);
  Serial.println("DONE WITH REFLOW!");
  delay(1000);

}



void loop() {

  setPoint = 0.0;

  Serial.println("Reflow operation completed. Turning off SSR");
  digitalWrite(SSR_PIN, LOW);
  while (true) {
    if (millis() % 1000 == 0) {
      temperature = thermocouple.readCelsius();
      if (isnan(temperature)) {
        Serial.println("Something wrong with thermocouple!");
      } else {
        Serial.print("C = ");
        Serial.println(temperature);
      }
    }
  }
  /*
    if (millis() % 1000 == 0) {
    temperature = thermocouple.readCelsius();
    if (isnan(temperature)) {
      Serial.println("Something wrong with thermocouple!");
    } else {
      Serial.print("C = ");
      Serial.println(temperature);
    }
    myPID.run();
    }

    digitalWrite(SSR_PIN, (millis() % 500 < outputVal) ? (HIGH) : (LOW) ) ; // 10 Hz 50% duty cycle
  */

}

void goToTemp(double desiredTemp, int reachTime, int holdTime) {

  setPoint = desiredTemp;

  temperature = thermocouple.readCelsius();

  unsigned long currentTime = millis();
  unsigned long endTime;


  // if current temp higher than desired temp, wait for it to cooldown
  temperature = thermocouple.readCelsius();
  if (temperature > desiredTemp) {
    Serial.println("Waiting for hot plate to cool off before starting");
    digitalWrite(SSR_PIN, LOW);
  }
  while (temperature > desiredTemp) {
    if (millis() % 1000 == 0) {
      temperature = thermocouple.readCelsius();
      if (isnan(temperature)) {
        Serial.println("Something wrong with thermocouple!");
      } else {
        Serial.print("C = ");
        Serial.println(temperature);
      }
      delay(1);
    }
  }

  Serial.print("Going to temp = ");
  Serial.println(desiredTemp);
  // get to desired temp140 first
  while (temperature < desiredTemp) {
    if (millis() % 1000 == 0) {
      temperature = thermocouple.readCelsius();
      if (isnan(temperature)) {
        Serial.println("Something wrong with thermocouple!");
      } else {
        Serial.print("C = ");
        Serial.println(temperature);
        delay(1);
      }
      myPID.run();
      digitalWrite(SSR_PIN, (millis() % 200 < outputVal) ? (HIGH) : (LOW) ) ; // 10 Hz 50% duty cycle

    }


  }

  endTime = millis();
  Serial.print("Time to reach desired temp = ");
  Serial.println((double)(endTime - currentTime) / 1000.0);

  // allow plate to cool to compensate for overshoot
  /*
  digitalWrite(SSR_PIN, LOW);
  delay(5000);

  temperature = thermocouple.readCelsius();
  while (temperature > desiredTemp) {
    if (millis() % 1000 == 0) {
      temperature = thermocouple.readCelsius();
      if (isnan(temperature)) {
        Serial.println("Something wrong with thermocouple!");
      } else {
        Serial.print("C = ");
        Serial.println(temperature);
        delay(1);
      }
    }
  }

  */


  Serial.print("Going to hold temp for ");
  Serial.print(holdTime);
  Serial.println(" seconds");


  // hold temp for certain amount of time
  currentTime = millis();
  endTime = currentTime + (long)(1000.0 * holdTime);

  while (millis() < endTime) {
    if (millis() % 1000 == 0) {
      temperature = thermocouple.readCelsius();
      if (isnan(temperature)) {
        Serial.println("Something wrong with thermocouple!");
      } else {
        Serial.print("C = ");
        Serial.println(temperature);
      }
      myPID.run();
      digitalWrite(SSR_PIN, (millis() % 200 < outputVal) ? (HIGH) : (LOW) ) ; // 10 Hz 50% duty cycle
      delay(1);

    }

  }

  endTime = millis();
  Serial.print("Time spent holding desired temp = ");
  Serial.println((double)(endTime - currentTime) / 1000.0);

}
