#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo gate
#define GATE_SERVO_PIN 9
Servo gateServo;

// Ultrasonic sensors
#define TRIG_ENTER A1
#define ECHO_ENTER A2
#define TRIG_EXIT 6
#define ECHO_EXIT 7

const int distanceThreshold = 10; // in cm

// IR parking sensors
#define IRPark1 1
#define IRPark2 11
#define IRPark3 12

// Smart lights
const int irLightPins[2] = {3, 5};
const int ledPins[2] = {4, 8};
unsigned long ledTimers[2] = {0, 0};
bool ledStates[2] = {false, false};
const unsigned long ledDuration = 5000;

// Gate timing
bool gateOpen = false;

// Gate control states
enum GateState { IDLE, ENTERING, EXITING };
GateState gateState = IDLE;

// Sensor cooldown timing
unsigned long lastEnterDetectTime = 0;
unsigned long lastExitDetectTime = 0;
const unsigned long sensorCooldown = 5000; // 5 seconds

void setup() {
  Serial.begin(9600);
  
  // IR parking
  pinMode(IRPark1, INPUT);
  pinMode(IRPark2, INPUT);
  pinMode(IRPark3, INPUT);

  // Ultrasonic sensors
  pinMode(TRIG_ENTER, OUTPUT);
  pinMode(ECHO_ENTER, INPUT);
  pinMode(TRIG_EXIT, OUTPUT);
  pinMode(ECHO_EXIT, INPUT);

  // Smart light setup
  for (int i = 0; i < 2; i++) {
    pinMode(irLightPins[i], INPUT);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Servo
  gateServo.attach(GATE_SERVO_PIN);
  gateServo.write(90);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
}

void loop() {
  unsigned long currentMillis = millis();

  // Smart lights
  for (int i = 0; i < 2; i++) {
    if (digitalRead(irLightPins[i]) == LOW) {
      digitalWrite(ledPins[i], HIGH);
      ledTimers[i] = currentMillis;
      ledStates[i] = true;
    }
    if (ledStates[i] && currentMillis - ledTimers[i] > ledDuration) {
      digitalWrite(ledPins[i], LOW);
      ledStates[i] = false;
    }
  }

  // Display available parking spots
  int available = 0;
  if (digitalRead(IRPark1) == HIGH) available++;
  if (digitalRead(IRPark2) == HIGH) available++;
  if (digitalRead(IRPark3) == HIGH) available++;

  lcd.setCursor(0, 0);
  lcd.print("Free Spots:     ");
  lcd.setCursor(0, 1);
  lcd.print(available);
  lcd.print(" available   ");

  // Gate logic with cooldown
  switch (gateState) {
    case IDLE:
      if (available > 0 && carDetectedWithCooldown(TRIG_ENTER, ECHO_ENTER, lastEnterDetectTime)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Car Entering");
        lcd.setCursor(0, 1);
        lcd.print("Opening Gate");
        gateServo.write(0);
        gateOpen = true;
        gateState = ENTERING;
      } else if (carDetectedWithCooldown(TRIG_EXIT, ECHO_EXIT, lastExitDetectTime)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Car Exiting");
        lcd.setCursor(0, 1);
        lcd.print("Opening Gate");
        gateServo.write(0);
        gateOpen = true;
        gateState = EXITING;
      }
      break;

    case ENTERING:
      if (carDetectedWithCooldown(TRIG_EXIT, ECHO_EXIT, lastExitDetectTime)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Entry Done");
        lcd.setCursor(0, 1);
        lcd.print("Closing Gate");
        gateServo.write(90);
        gateOpen = false;
        gateState = IDLE;
      }
      break;

    case EXITING:
      if (carDetectedWithCooldown(TRIG_ENTER, ECHO_ENTER, lastEnterDetectTime)) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Exit Done");
        lcd.setCursor(0, 1);
        lcd.print("Closing Gate");
        gateServo.write(90);
        gateOpen = false;
        gateState = IDLE;
      }
      break;
  }

  delay(100); // Small delay for stability
}

// Modified detection function with cooldown
bool carDetectedWithCooldown(int trigPin, int echoPin, unsigned long &lastDetectTime) {
  unsigned long currentMillis = millis();
  if (currentMillis - lastDetectTime < sensorCooldown) {
    return false;
  }

  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 20000); // 20ms timeout
  long distance = duration * 0.034 / 2;
  if (distance > 0 && distance < distanceThreshold) {
    lastDetectTime = currentMillis;
    return true;
  }
  return false;
}


