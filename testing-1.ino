#include <Stepper.h>

const int proximitySensorPin = 5;
const int LED_PIN = LED_BUILTIN; 
const int stepsPerRevolution = 200;

int stepCount = 0;
int dirStep = 1;

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  pinMode(LED_PIN, OUTPUT); 
  myStepper.setSpeed(60);
  pinMode(proximitySensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {

  int proximitySensorState = digitalRead(proximitySensorPin);

  myStepper.step(dirStep);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount++;

  if (proximitySensorState == LOW) {

    digitalWrite(LED_PIN, HIGH); 

    Serial.println("OBSTACLE DETECTED!!");
    Serial.println("Rotating Clockwise");
    myStepper.step(stepsPerRevolution); 

    if( stepCount > 500){
      stepCount = 0;
      dirStep = -dirStep; // Reverse direction
    }

    delayMicroseconds(50);

  } else {

    digitalWrite(LED_PIN, LOW);
    Serial.println("No Obstacle Detected");
    Serial.println("Rotating Counterclockwise");
    myStepper.step(-stepsPerRevolution);

    if( stepCount > 500){
      stepCount = 0;
      dirStep = -dirStep; // Reverse direction
    }

    delayMicroseconds(50);

  }

  delay(200);

}
