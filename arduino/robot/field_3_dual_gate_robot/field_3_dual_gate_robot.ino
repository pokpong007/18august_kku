#include <DINCRobot.h>

using namespace dinc_robot;

// Starting pose: lower-left blue entrance, facing north (toward mark 1).
const uint16_t DRIVE_30_CM_MS = settings::DRIVE_30_CM_MS;
const uint16_t DRIVE_40_CM_MS = settings::DRIVE_30_CM_MS * 4UL / 3UL;
const uint16_t SWITCH_PUSH_MS = 650;
const uint16_t GATE_WAIT_TIMEOUT_MS = 10000;

RobotCalibration calibration;
Robot robot(RobotPins(), calibration);

const Step route[] = {
    // Mark 1 then mark 5: enter the lower room and hit its limit switch.
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::TurnRight90, 0, 0},
    {Action::ForwardUntilObstacle, 2200, 12},
    {Action::PressSwitch, SWITCH_PUSH_MS, 0},
    {Action::Pause, 3200, 0},

    // Mark 6 then 7: reverse out and move north through the opened gate.
    {Action::Backward, DRIVE_40_CM_MS, 0},
    {Action::TurnRight90, 0, 0},
    {Action::WaitForGate, GATE_WAIT_TIMEOUT_MS, 32},
    {Action::Forward, DRIVE_40_CM_MS, 0},

    // Marks 2-3: cross the upper room and hit the second limit switch.
    {Action::TurnRight90, 0, 0},
    {Action::ForwardUntilObstacle, 2200, 12},
    {Action::PressSwitch, SWITCH_PUSH_MS, 0},
    {Action::Pause, 3200, 0},

    // Marks 4 and 8: reverse, turn north, and exit through the blue gate.
    {Action::Backward, DRIVE_40_CM_MS, 0},
    {Action::TurnLeft90, 0, 0},
    {Action::WaitForGate, GATE_WAIT_TIMEOUT_MS, 32},
    {Action::Forward, DRIVE_30_CM_MS, 0},
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
