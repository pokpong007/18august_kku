#ifndef DINC_ROBOT_PINS_H
#define DINC_ROBOT_PINS_H

#include <Arduino.h>

// Exact copy of the hardware mapping in DINCRobot.h/robot_pins.ino.
// Robot sketches must not assign any additional hardware to these pins.
namespace dinc_robot {
namespace pins {

constexpr uint8_t LED_PIN = LED_BUILTIN;

constexpr uint8_t LEFT_MOTOR_DIR_PIN = 2;
constexpr uint8_t LEFT_MOTOR_PWM_PIN = 5;
constexpr uint8_t RIGHT_MOTOR_DIR_PIN = 4;
constexpr uint8_t RIGHT_MOTOR_PWM_PIN = 6;

constexpr uint8_t IR_REMOTE_PIN = 3;
constexpr uint8_t LINE_LEFT_PIN = 7;
constexpr uint8_t LINE_RIGHT_PIN = 9;
constexpr uint8_t SERVO_PIN = 10;
constexpr uint8_t ULTRASONIC_TRIG_PIN = 12;
constexpr uint8_t ULTRASONIC_ECHO_PIN = 13;

constexpr uint8_t LDR_LEFT_PIN = A0;
constexpr uint8_t IR_OBSTACLE_LEFT_PIN = A1;
constexpr uint8_t IR_OBSTACLE_RIGHT_PIN = A2;
constexpr uint8_t LDR_RIGHT_PIN = A3;

constexpr uint8_t LINE_BLACK = HIGH;
constexpr uint8_t FALL_DANGER_STATE = HIGH;
constexpr uint8_t IR_BLOCKED = LOW;

}  // namespace pins
}  // namespace dinc_robot

#endif
