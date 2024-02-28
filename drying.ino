#include <Stepper.h>

// Define pin numbers
const int proximitySensorPin = 7;
const int LED_PIN = LED_BUILTIN; 

// Define stepper motor parameters
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

void setup() {
    pinMode(LED_PIN, OUTPUT); 
    myStepper.setSpeed(60);
    pinMode(proximitySensorPin, INPUT);
    Serial.begin(9600);
}

void loop() {
    // Read the state of the proximity sensor
    int proximitySensorState = digitalRead(proximitySensorPin);

    if (proximitySensorState == LOW) {
        // Obstacle detected
        digitalWrite(LED_PIN, HIGH); 
        Serial.println("OBSTACLE DETECTED!!");
        Serial.println("Rotating Clockwise");
        myStepper.step(stepsPerRevolution); 
        delay(500);
    } else {
        // No obstacle detected
        digitalWrite(LED_PIN, LOW);
        Serial.println("No Obstacle Detected");
        Serial.println("Rotating Counterclockwise");
        myStepper.step(-stepsPerRevolution);
        delay(500);
    }

    delay(200); // Delay for stability
}
