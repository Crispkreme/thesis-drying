#include <Stepper.h>

const int proximitySensorPin = 5;
const int LED_PIN = LED_BUILTIN; 
const int stepsPerRevolution = 200;

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  pinMode(LED_PIN, OUTPUT); 
  myStepper.setSpeed(60);
  pinMode(proximitySensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {

  int proximitySensorState = digitalRead(proximitySensorPin);

  if (proximitySensorState == LOW) {

    digitalWrite(LED_PIN, HIGH); 
    Serial.println("OBSTACLE DETECTED!!");
    Serial.println("Rotating Clockwise");
    myStepper.step(stepsPerRevolution); 
    delay(500);

  } else {

    digitalWrite(LED_PIN, LOW);
    Serial.println("No Obstacle Detected");
    Serial.println("Rotating Counterclockwise");
    myStepper.step(-stepsPerRevolution);
    delay(500);

  }

  delay(200);

}
