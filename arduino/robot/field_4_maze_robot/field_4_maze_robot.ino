#include <DINCRobot.h>

using namespace dinc_robot;

// Starting pose: at the ultrasonic start point, facing into the maze.
// The HC-SR04 is mounted on D10 servo, so it scans right/front/left and uses
// the right-hand rule. The two fixed IR sensors are angled forward and are
// used only as close-obstacle backup.
const float PATH_OPEN_CM = 24.0f;
const uint16_t CORNER_ADVANCE_MS = 260;
const uint16_t FORWARD_PULSE_MS = 220;
const int16_t STEERING_CORRECTION = 25;
const uint16_t FINISH_CONFIRM_MS = 500;
const uint32_t MAX_RUN_TIME_MS = 120000UL;

RobotCalibration calibration;
Robot robot(RobotPins(), calibration);
bool hasRun = false;

bool finishZoneConfirmed() {
  static uint32_t darkSince = 0;
  if (robot.bothLineSensorsActive()) {
    if (darkSince == 0) {
      darkSince = millis();
    }
    return millis() - darkSince >= FINISH_CONFIRM_MS;
  }
  darkSince = 0;
  return false;
}

bool turnAtOpening(bool turnRight) {
  if (!robot.runTimed(calibration.slowSpeed, calibration.slowSpeed,
                      CORNER_ADVANCE_MS, true)) {
    return false;
  }
  const Step turn = {turnRight ? Action::TurnRight90 : Action::TurnLeft90,
                     0, 0};
  if (!robot.execute(turn)) {
    return false;
  }
  robot.lookHeadCenter();
  return true;
}

bool pathOpen(float distanceCm) {
  // -1 means no echo inside the HC-SR04 timeout, which is also open space.
  return distanceCm < 0 || distanceCm > PATH_OPEN_CM;
}

bool driveForwardPulse() {
  const uint32_t startedAt = millis();
  while (millis() - startedAt < FORWARD_PULSE_MS) {
    if (finishZoneConfirmed()) {
      robot.stop();
      return true;
    }

    const bool closeOnLeft = robot.leftObstacleBlocked();
    const bool closeOnRight = robot.rightObstacleBlocked();
    if (closeOnLeft && !closeOnRight) {
      // Steer right, away from the left-side object.
      robot.setMotorSpeeds(calibration.driveSpeed,
                           calibration.driveSpeed - STEERING_CORRECTION);
    } else if (closeOnRight && !closeOnLeft) {
      // Steer left, away from the right-side object.
      robot.setMotorSpeeds(calibration.driveSpeed - STEERING_CORRECTION,
                           calibration.driveSpeed);
    } else {
      // Both sensors can see the two corridor walls at the same time. Keep
      // going straight rather than treating that normal case as an obstacle.
      robot.setMotorSpeeds(calibration.driveSpeed, calibration.driveSpeed);
    }
    delay(15);
  }
  robot.stop();
  return true;
}

bool solveMaze() {
  const uint32_t startedAt = millis();
  while (millis() - startedAt < MAX_RUN_TIME_MS) {
    if (finishZoneConfirmed()) {
      robot.stop();
      Serial.println(F("Maze finish zone detected"));
      return true;
    }

    robot.stop();
    const float rightDistance = robot.scanRightCm();
    const float frontDistance = robot.scanCenterCm();
    const float leftDistance = robot.scanLeftCm();
    robot.lookHeadCenter();

    const bool rightOpen = pathOpen(rightDistance);
    const bool frontOpen = pathOpen(frontDistance);
    const bool leftOpen = pathOpen(leftDistance);

    Serial.print(F("maze R/F/L = "));
    Serial.print(rightDistance);
    Serial.print(F(" / "));
    Serial.print(frontDistance);
    Serial.print(F(" / "));
    Serial.println(leftDistance);

    // Right-hand wall rule: take a right opening first, continue straight when
    // possible, otherwise take the left opening or turn around at a dead end.
    if (rightOpen) {
      if (!turnAtOpening(true)) {
        return false;
      }
    } else if (frontOpen) {
      // Move in short pulses and use the two angled IR modules only for small
      // steering corrections. Junction decisions come from the rotating head.
      if (!driveForwardPulse()) {
        return false;
      }
    } else if (leftOpen) {
      if (!turnAtOpening(false)) {
        return false;
      }
    } else {
      const Step around = {Action::TurnAround, 0, 0};
      if (!robot.execute(around)) {
        return false;
      }
      robot.lookHeadCenter();
    }
  }

  robot.stop();
  Serial.println(F("ABORT: maze time limit"));
  return false;
}

void setup() {
  // Edge safety is disabled here because the two downward sensors are reused
  // to recognise the large coloured END zone.
  calibration.enableEdgeSafety = false;
  calibration.enableObstacleStop = false;
  calibration.driveSpeed = 125;
  calibration.slowSpeed = 95;
  robot.setCalibration(calibration);
  robot.begin();
}

void loop() {
  if (hasRun) {
    return;
  }
  hasRun = true;
  robot.waitBeforeRun(4000);
  solveMaze();
  robot.stop();
}
