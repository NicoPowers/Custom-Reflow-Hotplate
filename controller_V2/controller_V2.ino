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

void goToTemp(double temp, int reachTime, int holdTime);
void ssrControl(int onTime);


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

  //if temperature is more than 4 degrees below or above setpoint, OUTPUT will be set to min or max respectively
//  myPID.setBangBang(20);
//  myPID.setTimeStep(1000);


  goToTemp(PREHEAT, 60, 100);
  //  Serial.println("DONE WITH PREHEAT!");
  delay(1000);
  goToTemp(SOAK, 10, 120);
  //  Serial.println("DONE WITH SOAK!");
  delay(1000);
  goToTemp(REFLOW, 10, 10);
  //  Serial.println("DONE WITH REFLOW!");
  delay(1000);

}



void loop() {

  setPoint = 0.0;

  //  Serial.println("Reflow operation completed. Turning off SSR");
  digitalWrite(SSR_PIN, LOW);
  while (true) {
    if (millis() % 1000 == 0) {
      temperature = thermocouple.readCelsius();
      if (isnan(temperature)) {
        //        Serial.println("Something wrong with thermocouple!");
      } else {
        //        Serial.print("C = ");
        //        Serial.println(temperature);
        Serial.print(millis() / 1000.0);
        Serial.print(',');
        Serial.print(temperature);
        Serial.print('\n');
      }
    }
  }

}

void goToTemp(double desiredTemp, int reachTime, int holdTime) {

  setPoint = desiredTemp;

  temperature = thermocouple.readCelsius();

  unsigned long currentTime = millis();
  unsigned long endTime;
  double feedBackError;

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
        //        Serial.println("Something wrong with thermocouple!");
      } else {
        //        Serial.print("C = ");
        //        Serial.println(temperature);
        Serial.print(millis() / 1000.0);
        Serial.print(',');
        Serial.print(temperature);
        Serial.print('\n');
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
        //        Serial.println("Something wrong with thermocouple!");
      } else {
        //        Serial.print("C = ");
        //        Serial.println(temperature);
        Serial.print(millis() / 1000.0);
        Serial.print(',');
        Serial.print(temperature);
        Serial.print('\n');
        feedBackError = KP * (desiredTemp - temperature);
        Serial.print("Feedback error = ");
        Serial.println(feedBackError);
        ssrControl(feedBackError*MAX_ON_TIME);
      }
          

    }


  }

  endTime = millis();
  Serial.print("Time to reach desired temp = ");
  Serial.println((double)(endTime - currentTime) / 1000.0);


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
        //        Serial.println("Something wrong with thermocouple!");
      } else {
        //        Serial.print("C = ");
        //        Serial.println(temperature);
        Serial.print(millis() / 1000.0);
        Serial.print(',');
        Serial.print(temperature);
        Serial.print('\n');
        feedBackError = KP * (desiredTemp - temperature);
        Serial.print("Feedback error = ");
        Serial.println(feedBackError);
        ssrControl(feedBackError*MAX_ON_TIME);

      }
    }

  }

  endTime = millis();
  Serial.print("Time spent holding desired temp = ");
  Serial.println((double)(endTime - currentTime) / 1000.0);

}

void ssrControl(int onTime) {
  Serial.print("Going to turn on for ");
  Serial.print(onTime);
  Serial.println(" seconds.");
  unsigned long startTime = millis();
  unsigned long endTime = startTime + (1000*onTime);
  digitalWrite(SSR_PIN, HIGH);
  while(millis() < endTime) {
    
  }
  digitalWrite(SSR_PIN, LOW);
}
