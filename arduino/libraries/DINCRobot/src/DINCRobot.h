#ifndef DINC_ROBOT_H
#define DINC_ROBOT_H

#include <Arduino.h>
#include <Servo.h>
#include "RobotPins.h"
#include "RobotSettings.h"

namespace dinc_robot {

struct RobotPins {
  uint8_t statusLed = pins::LED_PIN;
  uint8_t leftMotorDir = pins::LEFT_MOTOR_DIR_PIN;
  uint8_t leftMotorPwm = pins::LEFT_MOTOR_PWM_PIN;
  uint8_t rightMotorDir = pins::RIGHT_MOTOR_DIR_PIN;
  uint8_t rightMotorPwm = pins::RIGHT_MOTOR_PWM_PIN;
  // Keep every pin identical to DINCRobot.h/robot_pins.ino. D10 controls the
  // physical servo that turns the ultrasonic head.
  uint8_t irRemote = pins::IR_REMOTE_PIN;
  uint8_t lineLeft = pins::LINE_LEFT_PIN;
  uint8_t lineRight = pins::LINE_RIGHT_PIN;
  uint8_t servo = pins::SERVO_PIN;
  uint8_t ultrasonicTrig = pins::ULTRASONIC_TRIG_PIN;
  uint8_t ultrasonicEcho = pins::ULTRASONIC_ECHO_PIN;
  uint8_t ldrLeft = pins::LDR_LEFT_PIN;
  uint8_t obstacleLeft = pins::IR_OBSTACLE_LEFT_PIN;
  uint8_t obstacleRight = pins::IR_OBSTACLE_RIGHT_PIN;
  uint8_t ldrRight = pins::LDR_RIGHT_PIN;
};

struct RobotCalibration {
  int16_t driveSpeed = settings::DRIVE_SPEED;
  int16_t slowSpeed = settings::SLOW_SPEED;
  int16_t turnSpeed = settings::TURN_SPEED;
  int16_t arcInnerSpeed = settings::ROUNDABOUT_INNER_SPEED;
  int16_t arcOuterSpeed = settings::ROUNDABOUT_OUTER_SPEED;
  uint16_t turnLeft90Ms = settings::TURN_LEFT_90_MS;
  uint16_t turnRight90Ms = settings::TURN_RIGHT_90_MS;
  uint16_t turn180Ms = settings::TURN_180_MS;
  bool leftMotorForwardHigh = settings::LEFT_MOTOR_FORWARD_HIGH;
  bool rightMotorForwardHigh = settings::RIGHT_MOTOR_FORWARD_HIGH;
  uint8_t lineDangerState = pins::FALL_DANGER_STATE;
  uint8_t obstacleBlockedState = pins::IR_BLOCKED;
  bool enableEdgeSafety = true;
  bool enableObstacleStop = false;
};

enum class Action : uint8_t {
  Forward,
  Backward,
  TurnLeft90,
  TurnRight90,
  TurnAround,
  ArcLeft,
  ArcRight,
  ForwardUntilObstacle,
  WaitForGate,
  PressSwitch,
  Pause,
  Stop
};

struct Step {
  Action action;
  uint32_t durationMs;
  int16_t value;
};

class Robot {
 public:
  explicit Robot(const RobotPins &pins = RobotPins(),
                 const RobotCalibration &calibration = RobotCalibration())
      : pins_(pins), calibration_(calibration) {}

  void setCalibration(const RobotCalibration &calibration) {
    calibration_ = calibration;
  }

  void begin(unsigned long serialBaud = 115200) {
    Serial.begin(serialBaud);
    pinMode(pins_.leftMotorDir, OUTPUT);
    pinMode(pins_.leftMotorPwm, OUTPUT);
    pinMode(pins_.rightMotorDir, OUTPUT);
    pinMode(pins_.rightMotorPwm, OUTPUT);
    pinMode(pins_.lineLeft, INPUT);
    pinMode(pins_.lineRight, INPUT);
    pinMode(pins_.obstacleLeft, INPUT);
    pinMode(pins_.obstacleRight, INPUT);
    pinMode(pins_.ultrasonicTrig, OUTPUT);
    pinMode(pins_.ultrasonicEcho, INPUT);
    digitalWrite(pins_.ultrasonicTrig, LOW);
    headServo_.attach(pins_.servo);
    lookHeadCenter();
    stop();
  }

  void waitBeforeRun(uint32_t delayMs = 3000) {
    stop();
    Serial.println(F("Auto start countdown"));
    delay(delayMs);
  }

  void setMotorSpeeds(int16_t leftSpeed, int16_t rightSpeed) {
    leftSpeed = constrain(leftSpeed, static_cast<int16_t>(-255),
                          static_cast<int16_t>(255));
    rightSpeed = constrain(rightSpeed, static_cast<int16_t>(-255),
                           static_cast<int16_t>(255));

    const bool leftForward = leftSpeed >= 0;
    const bool rightForward = rightSpeed >= 0;
    digitalWrite(pins_.leftMotorDir,
                 leftForward == calibration_.leftMotorForwardHigh ? HIGH : LOW);
    digitalWrite(pins_.rightMotorDir,
                 rightForward == calibration_.rightMotorForwardHigh ? HIGH : LOW);
    analogWrite(pins_.leftMotorPwm, abs(leftSpeed));
    analogWrite(pins_.rightMotorPwm, abs(rightSpeed));
  }

  void stop() { setMotorSpeeds(0, 0); }

  bool leftLineActive() const {
    return digitalRead(pins_.lineLeft) == calibration_.lineDangerState;
  }

  bool rightLineActive() const {
    return digitalRead(pins_.lineRight) == calibration_.lineDangerState;
  }

  bool bothLineSensorsActive() const {
    return leftLineActive() && rightLineActive();
  }

  bool edgeDanger() const {
    return calibration_.enableEdgeSafety &&
           (leftLineActive() || rightLineActive());
  }

  bool obstacleBlocked() const {
    return digitalRead(pins_.obstacleLeft) == calibration_.obstacleBlockedState ||
           digitalRead(pins_.obstacleRight) == calibration_.obstacleBlockedState;
  }

  bool leftObstacleBlocked() const {
    return digitalRead(pins_.obstacleLeft) == calibration_.obstacleBlockedState;
  }

  bool rightObstacleBlocked() const {
    return digitalRead(pins_.obstacleRight) == calibration_.obstacleBlockedState;
  }

  int readLdrLeft() const { return analogRead(pins_.ldrLeft); }
  int readLdrRight() const { return analogRead(pins_.ldrRight); }

  float readDistanceCm() const {
    digitalWrite(pins_.ultrasonicTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(pins_.ultrasonicTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(pins_.ultrasonicTrig, LOW);

    const unsigned long duration =
        pulseIn(pins_.ultrasonicEcho, HIGH, 25000UL);
    if (duration == 0) {
      return -1.0f;
    }
    return duration / 58.0f;
  }

  void lookHeadLeft(uint16_t settleMs = settings::HEAD_SETTLE_MS) {
    setHeadAngle(settings::HEAD_LEFT_ANGLE, settleMs);
  }

  void lookHeadCenter(uint16_t settleMs = settings::HEAD_SETTLE_MS) {
    setHeadAngle(settings::HEAD_CENTER_ANGLE, settleMs);
  }

  void lookHeadRight(uint16_t settleMs = settings::HEAD_SETTLE_MS) {
    setHeadAngle(settings::HEAD_RIGHT_ANGLE, settleMs);
  }

  float scanLeftCm() {
    lookHeadLeft();
    return readStableDistanceCm();
  }

  float scanCenterCm() {
    lookHeadCenter();
    return readStableDistanceCm();
  }

  float scanRightCm() {
    lookHeadRight();
    return readStableDistanceCm();
  }

  float readStableDistanceCm() const {
    float samples[3];
    uint8_t validCount = 0;
    for (uint8_t i = 0; i < 3; ++i) {
      const float sample = readDistanceCm();
      if (sample > 0) {
        samples[validCount++] = sample;
      }
      delay(45);
    }
    if (validCount == 0) {
      return -1.0f;
    }
    if (validCount == 1) {
      return samples[0];
    }
    if (validCount == 2) {
      return (samples[0] + samples[1]) / 2.0f;
    }
    if (samples[0] > samples[1]) {
      const float temp = samples[0];
      samples[0] = samples[1];
      samples[1] = temp;
    }
    if (samples[1] > samples[2]) {
      const float temp = samples[1];
      samples[1] = samples[2];
      samples[2] = temp;
    }
    if (samples[0] > samples[1]) {
      samples[1] = samples[0];
    }
    return samples[1];
  }

  bool runTimed(int16_t leftSpeed, int16_t rightSpeed, uint32_t durationMs,
                bool allowObstacleContact = false) {
    const uint32_t startedAt = millis();
    setMotorSpeeds(leftSpeed, rightSpeed);
    while (millis() - startedAt < durationMs) {
      if (edgeDanger()) {
        stop();
        Serial.println(F("ABORT: edge/line safety sensor"));
        return false;
      }
      if (calibration_.enableObstacleStop && !allowObstacleContact &&
          obstacleBlocked()) {
        stop();
        Serial.println(F("ABORT: obstacle safety sensor"));
        return false;
      }
      delay(5);
    }
    stop();
    delay(70);
    return true;
  }

  bool waitForGate(uint16_t clearDistanceCm, uint32_t timeoutMs) {
    stop();
    const uint32_t startedAt = millis();
    uint8_t clearSamples = 0;
    while (millis() - startedAt < timeoutMs) {
      const float distance = readDistanceCm();
      if (distance < 0 || distance > clearDistanceCm) {
        if (++clearSamples >= 3) {
          return true;
        }
      } else {
        clearSamples = 0;
      }
      delay(80);
    }
    Serial.println(F("ABORT: gate wait timeout"));
    return false;
  }

  bool forwardUntilObstacle(uint16_t stopDistanceCm, uint32_t timeoutMs) {
    const uint32_t startedAt = millis();
    uint8_t blockedSamples = 0;
    setMotorSpeeds(calibration_.slowSpeed, calibration_.slowSpeed);
    while (millis() - startedAt < timeoutMs) {
      const float distance = readDistanceCm();
      const bool ultrasonicBlocked =
          distance > 0 && distance <= stopDistanceCm;
      // The two IR inputs can be side-facing (as in the maze), so the frontal
      // approach uses HC-SR04 only and does not assume a different mounting.
      if (ultrasonicBlocked) {
        if (++blockedSamples >= 3) {
          stop();
          return true;
        }
      } else {
        blockedSamples = 0;
      }
      if (edgeDanger()) {
        stop();
        Serial.println(F("ABORT: edge/line safety sensor"));
        return false;
      }
      delay(40);
    }
    stop();
    Serial.println(F("ABORT: obstacle approach timeout"));
    return false;
  }

  bool execute(const Step &step) {
    switch (step.action) {
      case Action::Forward:
        return runTimed(calibration_.driveSpeed, calibration_.driveSpeed,
                        step.durationMs);
      case Action::Backward:
        return runTimed(-calibration_.driveSpeed, -calibration_.driveSpeed,
                        step.durationMs, true);
      case Action::TurnLeft90:
        return runTimed(-calibration_.turnSpeed, calibration_.turnSpeed,
                        step.durationMs == 0 ? calibration_.turnLeft90Ms
                                             : step.durationMs,
                        true);
      case Action::TurnRight90:
        return runTimed(calibration_.turnSpeed, -calibration_.turnSpeed,
                        step.durationMs == 0 ? calibration_.turnRight90Ms
                                             : step.durationMs,
                        true);
      case Action::TurnAround:
        return runTimed(calibration_.turnSpeed, -calibration_.turnSpeed,
                        step.durationMs == 0 ? calibration_.turn180Ms
                                             : step.durationMs,
                        true);
      case Action::ArcLeft:
        return runTimed(calibration_.arcInnerSpeed,
                        calibration_.arcOuterSpeed, step.durationMs, true);
      case Action::ArcRight:
        return runTimed(calibration_.arcOuterSpeed,
                        calibration_.arcInnerSpeed, step.durationMs, true);
      case Action::ForwardUntilObstacle:
        return forwardUntilObstacle(step.value > 0 ? step.value : 12,
                                    step.durationMs > 0 ? step.durationMs
                                                        : 3000);
      case Action::WaitForGate:
        return waitForGate(step.value > 0 ? step.value : 30,
                           step.durationMs > 0 ? step.durationMs : 10000);
      case Action::PressSwitch:
        return runTimed(calibration_.slowSpeed, calibration_.slowSpeed,
                        step.durationMs, true);
      case Action::Pause:
        stop();
        delay(step.durationMs);
        return true;
      case Action::Stop:
        stop();
        return true;
    }
    return false;
  }

  bool runRoute(const Step *route, size_t stepCount) {
    for (size_t i = 0; i < stepCount; ++i) {
      Serial.print(F("Step "));
      Serial.println(i + 1);
      if (!execute(route[i])) {
        stop();
        return false;
      }
    }
    stop();
    Serial.println(F("Route complete"));
    return true;
  }

  const RobotCalibration &calibration() const { return calibration_; }

 private:
  void setHeadAngle(uint8_t angle, uint16_t settleMs) {
    headServo_.write(constrain(angle, static_cast<uint8_t>(0),
                               static_cast<uint8_t>(180)));
    delay(settleMs);
  }

  RobotPins pins_;
  RobotCalibration calibration_;
  Servo headServo_;
};

}  // namespace dinc_robot

#endif
