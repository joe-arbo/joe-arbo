#include <LiquidCrystal.h>

char in = '0';
char intruder = 0;

int buttonState = 0;
int count = 0;

bool ARMED = false;

LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

void setup(){
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(2, INPUT);

  lcd.begin(16, 2);
}

void loop(){

  if(Serial.available() > 0) in = Serial.read();
  if(in == 'a') ARMED = true;


  /*Bluetooth Appliance A*/
  if(in == '1'){
    digitalWrite(8,HIGH);
  }
  else if(in == '0'){  
    digitalWrite(8,LOW);
  }


  /*Photoresistor*/
  int sensorValue = analogRead(A5);
  //Serial.println(sensorValue);
  if (sensorValue < 400){
    digitalWrite(7, HIGH);
  }
  else{
    digitalWrite(7,LOW);
  }


  /*Button Security System*/
  buttonState = digitalRead(2);

  ARMED = true;

  if (ARMED && buttonState == HIGH){
    intruder = 'i';
     Serial.println(intruder);
  } 
  //can add else here to disarm
 


  /*Temperature Sensor*/
  float temp = analogRead(A4);
  float C = ((temp*5)/1024-0.5)/0.01;
  float F = ((9*C)/5)+32;


  /*LCD Display*/
  if(count % 2000 == 0)
  {
    Serial.println(F);
    lcd.setCursor(0, 0);
    lcd.print(F);
    lcd.print(" ");
    lcd.print((char)223);
    lcd.print("F");

    lcd.setCursor(0,1);
    if(F >= 70) {
      lcd.print("AC is ON  ");
    }
    else if (F < 70){
      lcd.print("Heat is ON");
    }
    //delay(1000);
  }
   count = count + 1;
}

  
