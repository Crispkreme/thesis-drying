#include <DHT.h>
#include <Stepper.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "HD44780_LCD_PCF8574.h"

#define DHT22_PIN A3
#define PROXIMITY_PIN A0
#define DISPLAY_DELAY_INIT 50
#define TOGGLE_SWITCH A1
#define BUTTON_STATE HIGH
#define START 1 
#define STOP 0
#define CW 1
#define CCW -1

int previousState;
int toggleSwitchState = 0;
bool motorDirectionForward = true;

const int speedPBInc = 22;
const int stopPB = 24;
const int upPositionPB = 28;
const int downPositionPB = 30;
const int speedPBDec = 26;

const int motorPin[] = {8, 9, 10, 11};
const int direction = 1;
const int stepsPerRevolution = 200;

int speedStep = 5;
int stepMinimum = 5;
int stepMaximum = 100;
int stopType = 0; // 0 = fully stopped , 1 = hold (consumes energy) 

int currentSpeed = 60;
int currentSPR = stepsPerRevolution;
int motorStopState = STOP;

HD44780LCD myLCD(4, 16, 0x27, &Wire);
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);
DHT dht22(DHT22_PIN, DHT22);

File myFile;

void setup() {

  previousState = 0;

  Serial.begin(9600);

  pinMode(7, OUTPUT);
  dht22.begin();
  myStepper.setSpeed(60);

  pinMode(PROXIMITY_PIN, INPUT);
  pinMode(speedPBInc, INPUT_PULLUP);
  pinMode(stopPB,INPUT_PULLUP);
  pinMode(upPositionPB,INPUT_PULLUP);
  pinMode(downPositionPB,INPUT_PULLUP);
  pinMode(speedPBDec,INPUT_PULLUP); 
  delay(DISPLAY_DELAY_INIT);
  
  delay(DISPLAY_DELAY_INIT);
  myLCD.PCF8574_LCDInit(myLCD.LCDCursorTypeOn);
  myLCD.PCF8574_LCDClearScreen();
  myLCD.PCF8574_LCDBackLightSet(true);
  myLCD.PCF8574_LCDGOTO(myLCD.LCDLineNumberOne, 0);

}

void lcdDisplay(String lcdString) {

  String gateOpenStringTitle = "FUZZY LOGIC";
  myLCD.PCF8574_LCDGOTO(myLCD.LCDLineNumberOne, 0);
  myLCD.PCF8574_LCDSendString(gateOpenStringTitle.c_str());
  myLCD.PCF8574_LCDSendChar(' ');
  for (int i = gateOpenStringTitle.length(); i < 16; i++) {
    myLCD.PCF8574_LCDSendChar(' ');
  }

  String gateOpenString = lcdString;
  myLCD.PCF8574_LCDGOTO(myLCD.LCDLineNumberTwo, 0);
  myLCD.PCF8574_LCDSendString(gateOpenString.c_str());
  myLCD.PCF8574_LCDSendChar(' ');

  for (int i = gateOpenString.length(); i < 16; i++) {
    myLCD.PCF8574_LCDSendChar(' ');
  }

}

void writeToFile(String humidityString) {

  Serial.println("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("Initialization failed!");
    while (true);
  }

  myFile = SD.open("data.txt", FILE_WRITE);

  if (myFile) {

    myFile.println(humidityString);
    myFile.close();
    Serial.println("Data written to file.");

  } else {

    Serial.println("Error opening file");

  }

}

void updateState() {

  if(digitalRead(stopPB) == LOW) {

    motorStopState = 1 - motorStopState;

    if(motorStopState  == 1) {

      Serial.println("MOTOR STATE STATUS: MOTOR STOP");
      stopMotor();

    } else {

      Serial.println("MOTOR STATE STATUS: MOTOR MOVING");
    }
  }

  if(digitalRead(speedPBInc) == LOW) {

    // MOTOR FORWARD DIRECTION
    motorStopState = START;

    if (motorStopState == 1) {

      Serial.println("MOTOR STATE STATUS: GATE CLOSE");

      forwardMotor();

    } else {

      Serial.println("MOTOR STATE STATUS: CHECK FORWARD BUTTON");

    }
  }

  if(digitalRead(speedPBDec) == LOW) {

    // MOTOR BACKWARD DIRECTION
    motorStopState = START;    

    if (motorStopState == 1) {

      Serial.println("MOTOR STATE STATUS: GATE OPEN");
      
      backwardMotor();

    } else {

      Serial.println("MOTOR STATE STATUS: CHECK BACKWARD BUTTON");

    }

  }  
}

void forwardMotor() {
  for(int i = 0; i < 25; i++) {
    myStepper.step(stepsPerRevolution);
  }
}

void backwardMotor() {
  for(int i = 24; i >= 0; i--) {
    myStepper.step(-stepsPerRevolution);
  }
}

void stopMotor() {
  if(stopType == 0)
  {
    for(int i = 0; i < 4; i++)
    {
      digitalWrite(motorPin[i], LOW);
    }
  }
}

void loop() {

    int actualState;
    int analogValue = analogRead(A1);
    float humidity = dht22.readHumidity();
    float temperature = dht22.readTemperature();
    int proximitySensorState = digitalRead(PROXIMITY_PIN);
    String message; 

    if (!isnan(humidity) && !isnan(temperature)) {

          Serial.print("Temperature (°C): ");
          Serial.println(temperature);

          Serial.print("Humidity (%): ");
          Serial.println(humidity);

          if (humidity >= 0 && humidity <= 35) {

            String message = "HUMIDITY: DRY";  
            lcdDisplay(message);

            Serial.println("HUMIDITY: DRY");
            Serial.print("Temperature (°C): " + String(temperature));       
            Serial.println("Humidity (%): " + String(humidity));

            writeToFile(message);

            delay(1000);

          } else if (humidity >= 36 && humidity <= 40) {

            String message = "HUMIDITY: COMFORTABLE";  
            lcdDisplay(message);

            Serial.println("HUMIDITY: COMFORTABLE");
            Serial.print("Temperature (°C): " + String(temperature));       
            Serial.println("Humidity (%): " + String(humidity));

            writeToFile(message);

            delay(1000);

          } else if (humidity >= 41 && humidity <= 65) {

            String message = "HUMIDITY: HUMID";  
            lcdDisplay(message);

            Serial.println("HUMIDITY: HUMID");
            Serial.print("Temperature (°C): " + String(temperature));       
            Serial.println("Humidity (%): " + String(humidity));

            writeToFile(message);

            delay(1000);

          } else if (humidity >= 66 && humidity <= 100) {

            String message = "HUMIDITY: STICKY";  
            lcdDisplay(message);

            Serial.println("HUMIDITY: STICKY");
            Serial.print("Temperature (°C): " + String(temperature));       
            Serial.println("Humidity (%): " + String(humidity));

            writeToFile(message);

            delay(1000);

          } else {

            String message = "HUMIDITY: INVALID";  
            lcdDisplay(message);

            Serial.println("HUMIDITY: INVALID");
            Serial.print("Temperature (°C): " + String(temperature));       
            Serial.println("Humidity (%): " + String(humidity));

            writeToFile(message);

            delay(1000);
          
          }

        } else {

            String message = "ERROR: PLEASE CHECK THE WIRING";  
            lcdDisplay(message);

            Serial.println("ERROR: PLEASE CHECK THE WIRING");

        }

    if (analogValue < 100) {

        // ON BUTTON BYPASS
        actualState = 1;
        Serial.println(actualState);
        digitalWrite(7, LOW);

        String message = "SYSTEM BYPASS";  
        lcdDisplay(message);
        updateState();
        delay(1000);

    } else if (analogValue < 900) {

        // LED INDICATOR
        actualState = 3;
        Serial.println(actualState);
        digitalWrite(7, HIGH);

        String message = "SYSTEM OFFLINE";  
        lcdDisplay(message);

        delay(1000);

    } else {

        // ACTIVE THE SENSOR
        actualState = 2;
        Serial.println(actualState);

        String message = "SYSTEM ONLINE";  
        lcdDisplay(message);

        Serial.println(proximitySensorState == LOW ? "HIGH" : "LOW");

        switch (proximitySensorState) {
          case HIGH:
            message = "GATE CLOSE";  
            lcdDisplay(message);
            Serial.println("GATE CLOSE");
            delay(1000);

            if(digitalRead(upPositionPB) == LOW) {
              forwardMotor();
              stopMotor();
            } else {
              Serial.println("Up Position HIGH");
            }

          break;
          case LOW:
            message = "GATE OPEN";  
            lcdDisplay(message);
            
            delay(1000);
            if(digitalRead(downPositionPB) == LOW) {
              backwardMotor();
              stopMotor();
            } else {
              Serial.println("Up Position HIGH");
            }
          break;
          default:
            stopMotor();
          break;
        }
    } 
}
