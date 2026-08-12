#ifndef ROBOT_PINS_H
#define ROBOT_PINS_H

#include <Arduino.h>

const byte LED_PIN = LED_BUILTIN;

const byte LEFT_MOTOR_DIR_PIN = 2;
const byte LEFT_MOTOR_PWM_PIN = 5;
const byte RIGHT_MOTOR_DIR_PIN = 4;
const byte RIGHT_MOTOR_PWM_PIN = 6;

const byte IR_REMOTE_PIN = 3;
const byte LINE_LEFT_PIN = 7;
const byte LINE_RIGHT_PIN = 9;
const byte SERVO_PIN = 10;
const byte ULTRASONIC_TRIG_PIN = 12;
const byte ULTRASONIC_ECHO_PIN = 13;

const byte LDR_LEFT_PIN = A0;
const byte IR_OBSTACLE_LEFT_PIN = A1;
const byte IR_OBSTACLE_RIGHT_PIN = A2;
const byte LDR_RIGHT_PIN = A3;

const byte LINE_BLACK = HIGH;
const byte FALL_DANGER_STATE = HIGH;
const byte IR_BLOCKED = LOW;

#endif