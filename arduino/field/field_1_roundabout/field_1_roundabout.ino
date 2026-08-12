#include <DINCField.h>

using namespace dinc_field;

// HC-SR04 sensors are placed at two successive checkpoints around the circle.
const uint8_t LAP_A_TRIG_PIN = 2;
const uint8_t LAP_A_ECHO_PIN = 3;
const uint8_t LAP_B_TRIG_PIN = 4;
const uint8_t LAP_B_ECHO_PIN = 5;
const uint8_t EXIT_BUTTON_PIN = 6;
const uint8_t RED_LED_PIN = 7;
const uint8_t GREEN_LED_PIN = 8;
const uint8_t ROOM_GATE_SERVO_PIN = 9;
const uint8_t EXIT_GATE_SERVO_PIN = 10;

const float DETECT_DISTANCE_CM = 18.0f;
const uint8_t REQUIRED_LAPS = 2;
const uint32_t CHECKPOINT_TIMEOUT_MS = 3500;
const uint32_t CLEAR_TIME_MS = 350;
const uint8_t GATE_CLOSED_ANGLE = 8;
const uint8_t GATE_OPEN_ANGLE = 95;

Ultrasonic checkpointA(LAP_A_TRIG_PIN, LAP_A_ECHO_PIN);
Ultrasonic checkpointB(LAP_B_TRIG_PIN, LAP_B_ECHO_PIN);
DebouncedButton exitButton(EXIT_BUTTON_PIN);
TimedGate roomGate(ROOM_GATE_SERVO_PIN, RED_LED_PIN, NO_PIN, GREEN_LED_PIN,
                   GATE_CLOSED_ANGLE, GATE_OPEN_ANGLE, 0);
TimedGate exitGate(EXIT_GATE_SERVO_PIN, NO_PIN, NO_PIN, NO_PIN,
                   GATE_CLOSED_ANGLE, GATE_OPEN_ANGLE, 0);

enum class LapState : uint8_t { WaitForA, WaitForB, WaitUntilClear };
LapState lapState = LapState::WaitForA;
uint8_t lapCount = 0;
uint32_t stateStartedAt = 0;
uint32_t bothClearSince = 0;

bool detected(float distanceCm) {
  return distanceCm > 1.0f && distanceCm <= DETECT_DISTANCE_CM;
}

void resetField() {
  lapCount = 0;
  lapState = LapState::WaitForA;
  stateStartedAt = millis();
  bothClearSince = 0;
  roomGate.reset();
  exitGate.reset();
  Serial.println(F("Field 1 reset"));
}

void updateLapCounter(bool atA, bool atB) {
  switch (lapState) {
    case LapState::WaitForA:
      if (atA) {
        lapState = LapState::WaitForB;
        stateStartedAt = millis();
      }
      break;

    case LapState::WaitForB:
      if (atB) {
        ++lapCount;
        Serial.print(F("Lap count: "));
        Serial.println(lapCount);
        lapState = LapState::WaitUntilClear;
        bothClearSince = 0;
        if (lapCount >= REQUIRED_LAPS) {
          roomGate.openNow();
        }
      } else if (millis() - stateStartedAt > CHECKPOINT_TIMEOUT_MS) {
        lapState = LapState::WaitForA;
      }
      break;

    case LapState::WaitUntilClear:
      if (!atA && !atB) {
        if (bothClearSince == 0) {
          bothClearSince = millis();
        } else if (millis() - bothClearSince >= CLEAR_TIME_MS) {
          lapState = LapState::WaitForA;
        }
      } else {
        bothClearSince = 0;
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  checkpointA.begin();
  checkpointB.begin();
  exitButton.begin();
  roomGate.begin();
  exitGate.begin();
  resetField();
}

void loop() {
  static uint32_t lastSampleAt = 0;
  exitButton.update();

  // Always consume the event. A press before two completed laps must not be
  // remembered and used later when the first gate opens.
  const bool exitPressed = exitButton.takePressed();
  if (roomGate.isOpen() && exitPressed) {
    exitGate.openNow();
    Serial.println(F("Exit button pressed: exit gate open"));
  }

  if (millis() - lastSampleAt >= 70) {
    lastSampleAt = millis();
    const bool atA = detected(checkpointA.readCm());
    delay(25);  // Prevent ultrasonic crosstalk.
    const bool atB = detected(checkpointB.readCm());
    updateLapCounter(atA, atB);
  }

  roomGate.update();
  exitGate.update();
}
