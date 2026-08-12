#include <DINCField.h>

using namespace dinc_field;

const uint8_t BRIDGE_BUTTON_PIN = 2;
const uint8_t DOOR_BUTTON_PIN = 3;
const uint8_t BRIDGE_RED_LED_PIN = 4;
const uint8_t BRIDGE_YELLOW_LED_PIN = 5;
const uint8_t BRIDGE_GREEN_LED_PIN = 6;
const uint8_t DOOR_RED_LED_PIN = 7;
const uint8_t DOOR_YELLOW_LED_PIN = 8;
const uint8_t BRIDGE_SERVO_PIN = 9;
const uint8_t DOOR_SERVO_PIN = 10;
const uint8_t DOOR_GREEN_LED_PIN = 11;

const uint32_t ACTION_DELAY_MS = 3000;
const uint8_t BRIDGE_RAISED_ANGLE = 15;
const uint8_t BRIDGE_LOWERED_ANGLE = 100;
const uint8_t DOOR_CLOSED_ANGLE = 8;
const uint8_t DOOR_OPEN_ANGLE = 95;

DebouncedButton bridgeButton(BRIDGE_BUTTON_PIN);
DebouncedButton doorButton(DOOR_BUTTON_PIN);
TimedGate bridge(BRIDGE_SERVO_PIN, BRIDGE_RED_LED_PIN,
                 BRIDGE_YELLOW_LED_PIN, BRIDGE_GREEN_LED_PIN,
                 BRIDGE_RAISED_ANGLE, BRIDGE_LOWERED_ANGLE,
                 ACTION_DELAY_MS);
TimedGate door(DOOR_SERVO_PIN, DOOR_RED_LED_PIN, DOOR_YELLOW_LED_PIN,
               DOOR_GREEN_LED_PIN, DOOR_CLOSED_ANGLE, DOOR_OPEN_ANGLE,
               ACTION_DELAY_MS);

void resetField() {
  bridge.reset();
  door.reset();
  Serial.println(F("Field 2 reset: bridge raised, door closed"));
}

void setup() {
  Serial.begin(115200);
  bridgeButton.begin();
  doorButton.begin();
  bridge.begin();
  door.begin();
  resetField();
}

void loop() {
  bridgeButton.update();
  doorButton.update();
  if (bridgeButton.takePressed()) {
    bridge.trigger();
    Serial.println(F("Bridge switch: 3 second countdown"));
  }
  if (doorButton.takePressed()) {
    door.trigger();
    Serial.println(F("Door switch: 3 second countdown"));
  }

  bridge.update();
  door.update();
}
