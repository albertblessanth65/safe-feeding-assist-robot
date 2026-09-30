final working 

#include <Servo.h>

// ---------------- PIN CONFIGURATION ----------------
const byte JOYSTICK_1_PIN = A0;
const byte JOYSTICK_2_PIN = A1;

const byte SERVO_1_PIN = 2;
const byte SERVO_2_PIN = 3;

// NEW: Servo lock/unlock button
const byte SERVO_LOCK_PIN = 8;

// NEW: Claw joystick switch and servo
const byte CLAW_SWITCH_PIN = 7;
const byte CLAW_SERVO_PIN = 9;

// ---------------- SERVO OBJECTS ----------------
Servo servo1;
Servo servo2;

// NEW: Claw servo object
Servo clawServo;

// ---------------- SERVO POSITIONS ----------------
// Both servos start at 90 degrees.
int servo1Angle = 90;
int servo2Angle = 90;

// ---------------- JOYSTICK SETTINGS ----------------
const int JOYSTICK_CENTER = 512;
const int DEAD_ZONE = 80;

// Amount the servo moves per control update
const int STEP_SIZE = 1;

// Control update speed
const unsigned long UPDATE_INTERVAL = 15;

unsigned long lastUpdateTime = 0;

// ---------------- LOCK BUTTON SETTINGS ----------------
bool servosLocked = false;

bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;

unsigned long lastDebounceTime = 0;

const unsigned long DEBOUNCE_DELAY = 50;

// ---------------- CLAW SETTINGS ----------------
const int CLAW_CLOSED_ANGLE = 0;
const int CLAW_OPEN_ANGLE = 90;

bool clawOpen = false;

bool lastClawButtonReading = HIGH;
bool stableClawButtonState = HIGH;

unsigned long lastClawDebounceTime = 0;

// --------------------------------------------------

void setup() {
  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);

  // Initial position
  servo1.write(servo1Angle);
  servo2.write(servo2Angle);

  // Button connected between D8 and GND
  pinMode(SERVO_LOCK_PIN, INPUT_PULLUP);

  // NEW: Claw
  clawServo.attach(CLAW_SERVO_PIN);
  clawServo.write(CLAW_CLOSED_ANGLE);

  // Joystick SW connected between D7 and GND
  pinMode(CLAW_SWITCH_PIN, INPUT_PULLUP);
}

// --------------------------------------------------

void loop() {

  // ==================================================
  // SERVO LOCK / UNLOCK BUTTON
  // ==================================================

  bool currentButtonReading = digitalRead(SERVO_LOCK_PIN);

  // Detect a change in the raw button signal
  if (currentButtonReading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  // Accept the reading only after it remains stable
  if ((millis() - lastDebounceTime) >= DEBOUNCE_DELAY) {

    if (currentButtonReading != stableButtonState) {

      stableButtonState = currentButtonReading;

      // Button has been PRESSED
      if (stableButtonState == LOW) {

        // Toggle servo lock state
        servosLocked = !servosLocked;
      }
    }
  }

  lastButtonReading = currentButtonReading;


  // ==================================================
  // CLAW OPEN / CLOSE — INDEPENDENT OF D8 LOCK
  // ==================================================

  bool currentClawButtonReading = digitalRead(CLAW_SWITCH_PIN);

  if (currentClawButtonReading != lastClawButtonReading) {
    lastClawDebounceTime = millis();
  }

  if ((millis() - lastClawDebounceTime) >= DEBOUNCE_DELAY) {

    if (currentClawButtonReading != stableClawButtonState) {

      stableClawButtonState = currentClawButtonReading;

      // Claw switch pressed
      if (stableClawButtonState == LOW) {

        clawOpen = !clawOpen;

        if (clawOpen) {
          clawServo.write(CLAW_OPEN_ANGLE);
        }
        else {
          clawServo.write(CLAW_CLOSED_ANGLE);
        }
      }
    }
  }

  lastClawButtonReading = currentClawButtonReading;


  // ==================================================
  // ORIGINAL SERVO CONTROL — UNCHANGED
  // ==================================================

  // Run control loop at fixed interval
  if (millis() - lastUpdateTime < UPDATE_INTERVAL) {
    return;
  }

  lastUpdateTime = millis();

  // Read joysticks
  int joystick1 = analogRead(JOYSTICK_1_PIN);
  int joystick2 = analogRead(JOYSTICK_2_PIN);

  // ==================================================
  // SERVO SAFETY LOCK
  // ==================================================

  // When locked, joystick commands cannot change
  // either servo position.
  if (servosLocked) {
    servo1.write(servo1Angle);
    servo2.write(servo2Angle);
    return;
  }

  // -------- JOYSTICK 1 → SERVO 1 --------

  if (joystick1 > JOYSTICK_CENTER + DEAD_ZONE) {
    servo1Angle += STEP_SIZE;
  }
  else if (joystick1 < JOYSTICK_CENTER - DEAD_ZONE) {
    servo1Angle -= STEP_SIZE;
  }

  // -------- JOYSTICK 2 → SERVO 2 --------

  if (joystick2 > JOYSTICK_CENTER + DEAD_ZONE) {
    servo2Angle += STEP_SIZE;
  }
  else if (joystick2 < JOYSTICK_CENTER - DEAD_ZONE) {
    servo2Angle -= STEP_SIZE;
  }

  // -------- SAFETY LIMITS --------

  servo1Angle = constrain(servo1Angle, 0, 180);
  servo2Angle = constrain(servo2Angle, 0, 180);

  // -------- UPDATE SERVOS --------

  servo1.write(servo1Angle);
  servo2.write(servo2Angle);
}


/* SERVO WORKING OR NOT 
#include <Servo.h>

Servo servo;

void setup() {
  servo.attach(7);
}

void loop() {
  for (int angle = 0; angle <= 180; angle++) {
    servo.write(angle);
    delay(15);
  }

  for (int angle = 180; angle >= 0; angle--) {
    servo.write(angle);
    delay(15);
  }
} */