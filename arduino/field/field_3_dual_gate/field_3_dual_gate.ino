#include <DINCField.h>

using namespace dinc_field;

const uint8_t BLUE_LIMIT_SWITCH_PIN = 2;
const uint8_t GREEN_LIMIT_SWITCH_PIN = 3;
const uint8_t BLUE_RED_LED_PIN = 4;
const uint8_t BLUE_GREEN_LED_PIN = 5;
const uint8_t GREEN_RED_LED_PIN = 6;
const uint8_t GREEN_GREEN_LED_PIN = 7;
const uint8_t BLUE_GATE_SERVO_PIN = 9;
const uint8_t GREEN_GATE_SERVO_PIN = 10;

const uint32_t GATE_DELAY_MS = 3000;
const uint8_t BLUE_GATE_CLOSED_ANGLE = 10;
const uint8_t BLUE_GATE_OPEN_ANGLE = 100;
const uint8_t GREEN_GATE_CLOSED_ANGLE = 10;
const uint8_t GREEN_GATE_OPEN_ANGLE = 100;

DebouncedButton blueSwitch(BLUE_LIMIT_SWITCH_PIN);
DebouncedButton greenSwitch(GREEN_LIMIT_SWITCH_PIN);
TimedGate blueGate(BLUE_GATE_SERVO_PIN, BLUE_RED_LED_PIN, NO_PIN,
                   BLUE_GREEN_LED_PIN, BLUE_GATE_CLOSED_ANGLE,
                   BLUE_GATE_OPEN_ANGLE, GATE_DELAY_MS);
TimedGate greenGate(GREEN_GATE_SERVO_PIN, GREEN_RED_LED_PIN, NO_PIN,
                    GREEN_GREEN_LED_PIN, GREEN_GATE_CLOSED_ANGLE,
                    GREEN_GATE_OPEN_ANGLE, GATE_DELAY_MS);

void resetField() {
  blueGate.reset();
  greenGate.reset();
  Serial.println(F("Field 3 reset: both gates closed"));
}

void setup() {
  Serial.begin(115200);
  blueSwitch.begin();
  greenSwitch.begin();
  blueGate.begin();
  greenGate.begin();
  resetField();
}

void loop() {
  blueSwitch.update();
  greenSwitch.update();
  if (blueSwitch.takePressed()) {
    blueGate.trigger();
    Serial.println(F("Blue switch: gate opens after 3 seconds"));
  }
  if (greenSwitch.takePressed()) {
    greenGate.trigger();
    Serial.println(F("Green switch: gate opens after 3 seconds"));
  }

  blueGate.update();
  greenGate.update();
}
