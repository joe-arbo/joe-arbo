// This uses Serial Monitor to display Range Finder distance readings ghghgh

// Include NewPing Library
#include "NewPing.h"

// Hook up HC-SR04 with Trig to Arduino Pin 9, Echo to Arduino pin 10
#define TRIGGER_PIN_1 23
#define ECHO_PIN_1 22

#define TRIGGER_PIN_2 24
#define ECHO_PIN_2 25

#define TRIGGER_PIN_3 27
#define ECHO_PIN_3 26

#define TRIGGER_PIN_4 29
#define ECHO_PIN_4 28

#define TRIGGER_PIN_5 31
#define ECHO_PIN_5 30

#define TRIGGER_PIN_6 33
#define ECHO_PIN_6 32

#define TRIGGER_PIN_7 35
#define ECHO_PIN_7 34

#define TRIGGER_PIN_8 37
#define ECHO_PIN_8 36

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
int stopper, tree1Bit;

double calculatePD(double error_now, double error_last) {
  //P(error_now) + D(error_now-error_last)

  double transError = calculateTranslationalError(((distance7 + distance8) / 2), ((distance3 + distance4) / 2), 6);
  //Serial.print("Trans Error: ");
  //Serial.println(transError);

  //transError pos: move right
  //transError neg: move left
  double transThreshold = 1.5;
  double avgRight = (distance7 + distance8) / 2;
  double avgLeft = (distance3 + distance4) / 2;
  //if (transError > transThreshold){
  if ((avgRight > avgLeft) && (avgRight - avgLeft) > transThreshold) {
    Serial.println("Move Right");
    //calculateWheelSpeed
  } else
    //if (transError < (-1 * transThreshold)){
    if ((avgLeft > avgRight) && (avgLeft - avgRight) > transThreshold) {
      Serial.println("Move Left");
      //calculateWheelSpeed
    }
  //double rotError = calculateRotationalError(distance7, distance8, 0);

}

double calculateWheelSpeed(double T_Base, double translational_correction, double rotational_correction) {

}

double calculateTranslationalError(double avgLeft, double avgRight, double desiredDist) {
  //get average of sensor distances first
  //average - desired

  //go left
  if (avgLeft > avgRight) {
    return desiredDist - avgLeft;
  } else //go right
    if (avgLeft < avgRight) {
      return avgRight - desiredDist;
    }
}

double calculateRotationalError(double distSensor1, double distSensor2, double desiredAngle) {
  //right triangle math goes here
  int threshold = 1;

  //if this is pos: turn counter clockwise
  //if this is neg: turn clockwise
  int diff = distSensor1 - distSensor2;

  while (diff > threshold) {
    Serial.println("Turn counter clockwise: decrease wheel speed of left wheels");
  }
  while (diff < (-1 * threshold)) {
    Serial.println("Turn clockwise: decrease wheel speed of right wheels");
  }
  return 0.0;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  stopper = 0;
  tree1Bit = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  // Send ping, get distance in cm
  distance1 = sonar1.ping_cm();
  distance2 = sonar2.ping_cm();
  distance3 = sonar3.ping_cm();
  distance4 = sonar4.ping_cm();
  distance5 = sonar5.ping_cm();
  distance6 = sonar6.ping_cm();
  distance7 = sonar7.ping_cm();
  distance8 = sonar8.ping_cm();

  // Send results to Serial Monitor
  /*
    Serial.print("Distance1 = ");

    if (distance1 >= 400 || distance1 <= 1)
    {
    Serial.println("Out of range");
    }
    else
    {
    Serial.print(distance1);
    Serial.println(" cm");
    }
    // SENSOR 2
    Serial.print("Distance2 = ");

    if (distance2 >= 400 || distance2 <= 1)
    {
    Serial.println("Out of range");
    }
    else
    {
    Serial.print(distance2);
    Serial.println(" cm");
    }

    // SENSOR 3
    Serial.print("Distance3 = ");

    if (distance3 >= 400 || distance3 <= 1)
    {
    Serial.println("Out of range");
    }
    else
    {
    Serial.print(distance3);
    Serial.println(" cm");
    }

    // SENSOR 4
    Serial.print("Distance4 = ");

    if (distance4 >= 400 || distance4 <= 1)
    {
    Serial.println("Out of range");
    }
    else
    {
    Serial.print(distance4);
    Serial.println(" cm");
    }

    // SENSOR 5
    Serial.print("Distance5 = ");

    if (distance5 >= 400 || distance5 <= 1)
    {
    Serial.println("Out of range");
    }
    else
    {
    Serial.print(distance5);
    Serial.println(" cm");
    }

    // SENSOR 6
    Serial.print("Distance6 = ");

    if (distance6 >= 400 || distance6 <= 1)
    {
    Serial.println("Out of range");
    }
    else
    {
    Serial.print(distance6);
    Serial.println(" cm");
    }

    ///////////////// SENSOR 7
    Serial.print("Distance7 = ");

    if (distance7 >= 400 || distance7 <= 1)
    {
    Serial.println("Out of range");
    }
    else
    {
    Serial.print(distance7);
    Serial.println(" cm");
    }


    ///////////// SENSOR 8
    Serial.print("Distance8 = ");

    if (distance8 >= 400 || distance8 <= 1)
    {
    Serial.println("Out of range");
    }
    else
    {
    Serial.print(distance8);
    Serial.println(" cm");
    }


    calculatePD(0,0);
  */

  //double Speed_FR, Speed_FL, Speed_RR, Speed_RL;
  //double P, D;
  //double T_Base;
  //Grab the state (from pi)
  //Switch/If Statement for all the states (since we're not using multiple files)
  //Find corresponding T_Base (set value per state)
  //Calculate the error (translational and rotational, depending on state)
  //Calculate the speed per wheel with errors and T_Base
  //output speed, set T_Last to T_E

  
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
  delay(5000);
  
  
  /*
  if (distance1 < 15) {
    //Serial.flush();
    Serial.println("S");
    delay(50);
    if (tree1Bit == 0) {
      //Serial.flush();
      Serial.println("A");
      tree1Bit = 1;
      delay(50);
    }
    
    if (stopper == 0) {
      stopper = 1;
      while (stopper < 164) {
        //Serial.flush();
        Serial.println("C");
        stopper = stopper + 1;
        delay(50);
        
      }
      //Serial.flush();
      Serial.println("S");
      Serial.println("Fl");
    }
  }
  else {
    //Serial.flush();
    Serial.println("F");
  }
  delay(50);
  */
  
  
}