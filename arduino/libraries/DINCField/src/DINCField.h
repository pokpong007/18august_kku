#ifndef DINC_FIELD_H
#define DINC_FIELD_H

#include <Arduino.h>
#include <Servo.h>

namespace dinc_field {

const uint8_t NO_PIN = 255;

class DebouncedButton {
 public:
  DebouncedButton(uint8_t pin, uint16_t debounceMs = 35)
      : pin_(pin), debounceMs_(debounceMs) {}

  void begin() {
    pinMode(pin_, INPUT_PULLUP);
    rawState_ = digitalRead(pin_);
    stableState_ = rawState_;
    changedAt_ = millis();
  }

  void update() {
    const uint8_t sample = digitalRead(pin_);
    if (sample != rawState_) {
      rawState_ = sample;
      changedAt_ = millis();
    }
    if (millis() - changedAt_ >= debounceMs_ && stableState_ != rawState_) {
      stableState_ = rawState_;
      if (stableState_ == LOW) {
        pressedEvent_ = true;
      }
    }
  }

  bool takePressed() {
    const bool result = pressedEvent_;
    pressedEvent_ = false;
    return result;
  }

  bool isPressed() const { return stableState_ == LOW; }

 private:
  uint8_t pin_;
  uint16_t debounceMs_;
  uint8_t rawState_ = HIGH;
  uint8_t stableState_ = HIGH;
  uint32_t changedAt_ = 0;
  bool pressedEvent_ = false;
};

class Ultrasonic {
 public:
  Ultrasonic(uint8_t trigPin, uint8_t echoPin)
      : trigPin_(trigPin), echoPin_(echoPin) {}

  void begin() {
    pinMode(trigPin_, OUTPUT);
    pinMode(echoPin_, INPUT);
    digitalWrite(trigPin_, LOW);
  }

  float readCm(uint32_t timeoutUs = 25000UL) const {
    digitalWrite(trigPin_, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin_, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin_, LOW);
    const unsigned long duration = pulseIn(echoPin_, HIGH, timeoutUs);
    return duration == 0 ? -1.0f : duration / 58.0f;
  }

 private:
  uint8_t trigPin_;
  uint8_t echoPin_;
};

enum class GateState : uint8_t { Armed, Countdown, Open };

class TimedGate {
 public:
  TimedGate(uint8_t servoPin, uint8_t redLedPin, uint8_t yellowLedPin,
            uint8_t greenLedPin, uint8_t closedAngle, uint8_t openAngle,
            uint32_t delayMs)
      : servoPin_(servoPin),
        redLedPin_(redLedPin),
        yellowLedPin_(yellowLedPin),
        greenLedPin_(greenLedPin),
        closedAngle_(closedAngle),
        openAngle_(openAngle),
        delayMs_(delayMs) {}

  void begin() {
    beginLed(redLedPin_);
    beginLed(yellowLedPin_);
    beginLed(greenLedPin_);
    servo_.attach(servoPin_);
    reset();
  }

  void reset() {
    state_ = GateState::Armed;
    stateStartedAt_ = millis();
    servo_.write(closedAngle_);
    showRed();
  }

  void trigger() {
    if (state_ != GateState::Armed) {
      return;
    }
    state_ = GateState::Countdown;
    stateStartedAt_ = millis();
    showCountdown();
  }

  void openNow() {
    state_ = GateState::Open;
    stateStartedAt_ = millis();
    servo_.write(openAngle_);
    showGreen();
  }

  void update() {
    if (state_ == GateState::Countdown &&
        millis() - stateStartedAt_ >= delayMs_) {
      openNow();
    }
  }

  bool isOpen() const { return state_ == GateState::Open; }
  GateState state() const { return state_; }

 private:
  static void beginLed(uint8_t pin) {
    if (pin != NO_PIN) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }
  }

  static void writeLed(uint8_t pin, bool on) {
    if (pin != NO_PIN) {
      digitalWrite(pin, on ? HIGH : LOW);
    }
  }

  void showRed() {
    writeLed(redLedPin_, true);
    writeLed(yellowLedPin_, false);
    writeLed(greenLedPin_, false);
  }

  void showCountdown() {
    writeLed(redLedPin_, yellowLedPin_ == NO_PIN);
    writeLed(yellowLedPin_, true);
    writeLed(greenLedPin_, false);
  }

  void showGreen() {
    writeLed(redLedPin_, false);
    writeLed(yellowLedPin_, false);
    writeLed(greenLedPin_, true);
  }

  uint8_t servoPin_;
  uint8_t redLedPin_;
  uint8_t yellowLedPin_;
  uint8_t greenLedPin_;
  uint8_t closedAngle_;
  uint8_t openAngle_;
  uint32_t delayMs_;
  uint32_t stateStartedAt_ = 0;
  GateState state_ = GateState::Armed;
  Servo servo_;
};

}  // namespace dinc_field

#endif
