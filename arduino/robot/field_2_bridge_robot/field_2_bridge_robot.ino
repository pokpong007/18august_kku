#include <DINCRobot.h>

using namespace dinc_robot;

// Starting pose: bottom-left IN area, facing east (toward route marks 1-2).
const uint16_t DRIVE_30_CM_MS = settings::DRIVE_30_CM_MS;
const uint16_t DRIVE_SHORT_MS = settings::DRIVE_30_CM_MS / 2;
const uint16_t SWITCH_PUSH_MS = 650;
const uint16_t ACTION_WAIT_MS = 500;
const uint16_t GATE_WAIT_TIMEOUT_MS = 10000;

RobotCalibration calibration;
Robot robot(RobotPins(), calibration);

const Step route[] = {
    // Marks 1-2: travel east along the lower lane.
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::Forward, DRIVE_30_CM_MS, 0},

    // Marks 3-5: turn north and press the upper-right switch.
    {Action::TurnLeft90, 0, 0},
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::ForwardUntilObstacle, 1800, 12},
    {Action::PressSwitch, SWITCH_PUSH_MS, 0},
    {Action::Pause, ACTION_WAIT_MS, 0},
    {Action::Backward, DRIVE_SHORT_MS, 0},

    // Marks 6-9: cross the opened gate, go south, then west to switch 2.
    {Action::TurnLeft90, 0, 0},
    {Action::WaitForGate, GATE_WAIT_TIMEOUT_MS, 32},
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::TurnLeft90, 0, 0},
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::TurnRight90, 0, 0},
    {Action::ForwardUntilObstacle, 1800, 12},
    {Action::PressSwitch, SWITCH_PUSH_MS, 0},
    {Action::Pause, ACTION_WAIT_MS, 0},

    // Marks 10-13: back away, go north, and cross the lowered bridge.
    {Action::Backward, DRIVE_30_CM_MS, 0},
    {Action::TurnRight90, 0, 0},
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::TurnLeft90, 0, 0},
    {Action::WaitForGate, GATE_WAIT_TIMEOUT_MS, 35},
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::Forward, DRIVE_SHORT_MS, 0},
    {Action::Stop, 0, 0},
};

bool hasRun = false;

void setup() {
  calibration.enableEdgeSafety = true;
  calibration.enableObstacleStop = false;
  robot.setCalibration(calibration);
  robot.begin();
}

void loop() {
  if (hasRun) {
    return;
  }
  hasRun = true;
  robot.waitBeforeRun(3000);
  robot.runRoute(route, sizeof(route) / sizeof(route[0]));
}
