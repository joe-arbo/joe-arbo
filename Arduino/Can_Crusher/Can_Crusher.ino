#include <LiquidCrystal.h>
#include <millisDelay.h>

#define PISTON 2
#define pDelay 1000
#define crush 1500
#define retract 1000

millisDelay pistonDelay;
millisDelay crushTime;
millisDelay retractionTime;

int sensorValue = 0;
int cansCrushed = 0;
bool canPresent = false;
bool DEBUGGING = true; 

LiquidCrystal lcd(12, 11, 4, 5, 6, 7);

void setup() {
  Serial.begin(9600);
  pinMode(PISTON, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("Cans Crushed: ");
  lcd.setCursor(0,1);
  lcd.print(cansCrushed);
}

void loop() {
  sensorValue = analogRead(A4);
  if(DEBUGGING) Serial.println(sensorValue);

  if(sensorValue <= 50){
    digitalWrite(PISTON, LOW);
  }
  else if(sensorValue < 250 && !canPresent){
    pistonDelay.start(pDelay);
    canPresent = true;
  }

  if(pistonDelay.justFinished()){
    pistonDelay.stop();
    crushTime.start(crush);
  }
    
  if(crushTime.remaining() > 0){
     digitalWrite(PISTON, HIGH);
  }
  else if (crushTime.justFinished()){
     digitalWrite(PISTON, LOW);
     cansCrushed = cansCrushed + 1;
     lcd.setCursor(0,1);
     lcd.print(cansCrushed);
     retractionTime.start(retract);
     crushTime.stop();
  }
  else if (retractionTime.justFinished()){
     canPresent = false;
  }

  if(DEBUGGING){
    if(canPresent) Serial.println("True");
    else Serial.println("False"); 
  }
}
