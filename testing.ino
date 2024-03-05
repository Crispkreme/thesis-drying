#include <Stepper.h>

const int stepsPerRevolution = 200; // Assuming your stepper motor has 200 steps per revolution
const int IRPin = 5;
const int BUILTIN_LED = 13;

Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
  myStepper.setSpeed(50);
  pinMode(IRPin, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(IRPin) == LOW) {
    digitalWrite(BUILTIN_LED, HIGH);

    // Calculate the number of steps for a 20-turn movement
    int stepsFor20Turns = stepsPerRevolution * 20;

    // Rotate stepper motor for 20 turns clockwise
    Serial.println("Clockwise");
    myStepper.step(stepsFor20Turns);
    delay(400);
  } else {
    digitalWrite(BUILTIN_LED, LOW);

    // Rotate stepper motor for 20 turns counterclockwise
    Serial.println("Counterclockwise");
    myStepper.step(-stepsFor20Turns);
    delay(400);
  }
} 
