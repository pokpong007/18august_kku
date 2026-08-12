#ifndef DINC_ROBOT_H
#define DINC_ROBOT_H

#include <Arduino.h>
#include "robot_pins.h"

inline void setupMotors() {
  pinMode(LEFT_MOTOR_DIR_PIN, OUTPUT);
  pinMode(LEFT_MOTOR_PWM_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PWM_PIN, OUTPUT);
}

inline void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  digitalWrite(LEFT_MOTOR_DIR_PIN, leftSpeed >= 0 ? HIGH : LOW);
  digitalWrite(RIGHT_MOTOR_DIR_PIN, rightSpeed >= 0 ? LOW : HIGH);
  analogWrite(LEFT_MOTOR_PWM_PIN, abs(leftSpeed));
  analogWrite(RIGHT_MOTOR_PWM_PIN, abs(rightSpeed));
}

inline void moveForward(int speedValue) {
  setMotorSpeeds(speedValue, speedValue);
}

inline void moveBackward(int speedValue) {
  setMotorSpeeds(-speedValue, -speedValue);
}

inline void turnLeft(int speedValue) {
  setMotorSpeeds(-speedValue, speedValue);
}

inline void turnRight(int speedValue) {
  setMotorSpeeds(speedValue, -speedValue);
}

inline void stopMotors() {
  setMotorSpeeds(0, 0);
}

inline void setupSafetySensors() {
  pinMode(LINE_LEFT_PIN, INPUT);
  pinMode(LINE_RIGHT_PIN, INPUT);
  pinMode(IR_OBSTACLE_LEFT_PIN, INPUT);
  pinMode(IR_OBSTACLE_RIGHT_PIN, INPUT);
}

inline bool leftFallDanger() {
  return digitalRead(LINE_LEFT_PIN) == FALL_DANGER_STATE;
}

inline bool rightFallDanger() {
  return digitalRead(LINE_RIGHT_PIN) == FALL_DANGER_STATE;
}

inline bool fallDanger() {
  return leftFallDanger() || rightFallDanger();
}

inline bool leftObstacleBlocked() {
  return digitalRead(IR_OBSTACLE_LEFT_PIN) == IR_BLOCKED;
}

inline bool rightObstacleBlocked() {
  return digitalRead(IR_OBSTACLE_RIGHT_PIN) == IR_BLOCKED;
}

inline bool closeObstacleBlocked() {
  return leftObstacleBlocked() || rightObstacleBlocked();
}

inline void setupUltrasonic() {
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
}

inline float readDistanceCm() {
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, 30000);
  if (duration == 0) {
    return -1.0;
  }
  return duration / 58.0;
}

#endif