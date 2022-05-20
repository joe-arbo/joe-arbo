// This uses Serial Monitor to display Range Finder distance readings ghghgh

// Include NewPing Library
#include "NewPing.h"

// Hook up HC-SR04 with Trig to Arduino Pin 9, Echo to Arduino pin 10
#define TRIGGER_PIN_1 24
#define ECHO_PIN_1 22

#define TRIGGER_PIN_2 27
#define ECHO_PIN_2 29

#define TRIGGER_PIN_3 23
#define ECHO_PIN_3 25

#define TRIGGER_PIN_4 33
#define ECHO_PIN_4 31

#define TRIGGER_PIN_5 37
#define ECHO_PIN_5 35

#define TRIGGER_PIN_6 34
#define ECHO_PIN_6 36

#define TRIGGER_PIN_7 30
#define ECHO_PIN_7 32

#define TRIGGER_PIN_8 28
#define ECHO_PIN_8 26

// Maximum distance we want to ping for (in centimeters).
#define MAX_DISTANCE 400

// NewPing setup of pins and maximum distance.
NewPing sonar1(TRIGGER_PIN_1, ECHO_PIN_1, MAX_DISTANCE);
NewPing sonar2(TRIGGER_PIN_2, ECHO_PIN_2, MAX_DISTANCE);
NewPing sonar3(TRIGGER_PIN_3, ECHO_PIN_3, MAX_DISTANCE);
NewPing sonar4(TRIGGER_PIN_4, ECHO_PIN_4, MAX_DISTANCE);
NewPing sonar5(TRIGGER_PIN_5, ECHO_PIN_5, MAX_DISTANCE);
NewPing sonar6(TRIGGER_PIN_6, ECHO_PIN_6, MAX_DISTANCE);
NewPing sonar7(TRIGGER_PIN_7, ECHO_PIN_7, MAX_DISTANCE);
NewPing sonar8(TRIGGER_PIN_8, ECHO_PIN_8, MAX_DISTANCE);
float duration, distance1, distance2, distance3, distance4, distance5, distance6, distance7, distance8;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  distance1 = sonar1.ping_cm();
    distance2 = sonar2.ping_cm();
    distance3 = sonar3.ping_cm();
    distance4 = sonar4.ping_cm();
    distance5 = sonar5.ping_cm();
    distance6 = sonar6.ping_cm();
    distance7 = sonar7.ping_cm();
    distance8 = sonar8.ping_cm();

    Serial.print("Sensor1: ");
    Serial.println(distance1);
    Serial.print("Sensor2: ");
    Serial.println(distance2);
    Serial.print("Sensor3: ");
    Serial.println(distance3);
    Serial.print("Sensor4: ");
    Serial.println(distance4);
    Serial.print("Sensor5: ");
    Serial.println(distance5);
    Serial.print("Sensor6: ");
    Serial.println(distance6);
    Serial.print("Sensor7: ");
    Serial.println(distance7);
    Serial.print("Sensor8: ");
    Serial.println(distance8);
    Serial.println("/////////////////////////////////////");
    delay(2000);
}
