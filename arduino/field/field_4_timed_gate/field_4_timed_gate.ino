#include <DINCField.h>

using namespace dinc_field;

const uint8_t SENSOR_1_TRIG_PIN = 2;
const uint8_t SENSOR_1_ECHO_PIN = 3;
const uint8_t BUZZER_PIN = 4;
const uint8_t GATE_1_RED_LED_PIN = 6;
const uint8_t GATE_1_GREEN_LED_PIN = 7;
const uint8_t GATE_1_SERVO_PIN = 9;

// The confirmed IoT inventory has three HC-SR04 modules total: two are used by
// field 1, so field 4 intentionally contains one sensor and one gate.
const float PRESENCE_DISTANCE_CM = 18.0f;
const uint32_t PRESENCE_CONFIRM_MS = 250;
const uint32_t OPEN_DELAY_MS = 3000;
const uint8_t GATE_CLOSED_ANGLE = 8;
const uint8_t GATE_OPEN_ANGLE = 95;

Ultrasonic sensor1(SENSOR_1_TRIG_PIN, SENSOR_1_ECHO_PIN);
TimedGate gate1(GATE_1_SERVO_PIN, GATE_1_RED_LED_PIN, NO_PIN,
                GATE_1_GREEN_LED_PIN, GATE_CLOSED_ANGLE, GATE_OPEN_ANGLE,
                OPEN_DELAY_MS);

uint32_t detectedSince1 = 0;

bool presenceConfirmed(float distanceCm, uint32_t &detectedSince) {
  const bool present =
      distanceCm > 1.0f && distanceCm <= PRESENCE_DISTANCE_CM;
  if (!present) {
    detectedSince = 0;
    return false;
  }
  if (detectedSince == 0) {
    detectedSince = millis();
  }
  return millis() - detectedSince >= PRESENCE_CONFIRM_MS;
}

void resetField() {
  detectedSince1 = 0;
  gate1.reset();
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println(F("Field 4 reset: timed gate armed"));
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  sensor1.begin();
  gate1.begin();
  resetField();
}

void loop() {
  static uint32_t lastSampleAt = 0;
  if (millis() - lastSampleAt >= 70) {
    lastSampleAt = millis();
    if (presenceConfirmed(sensor1.readCm(), detectedSince1)) {
      gate1.trigger();
    }
  }

  gate1.update();
  digitalWrite(BUZZER_PIN,
               gate1.state() == GateState::Countdown ? HIGH : LOW);
}
