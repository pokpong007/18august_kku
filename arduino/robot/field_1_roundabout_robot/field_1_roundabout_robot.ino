#include <DINCRobot.h>

using namespace dinc_robot;

// Starting pose: in the IN zone, facing toward the roundabout.
// Shared movement values are in DINCRobot/src/RobotSettings.h.
const uint16_t DRIVE_30_CM_MS = settings::DRIVE_30_CM_MS;
const uint16_t ROUNDABOUT_LAP_MS = settings::ROUNDABOUT_ONE_LAP_MS;
const uint16_t SWITCH_PUSH_MS = 650;
const uint16_t GATE_WAIT_TIMEOUT_MS = 10000;

RobotCalibration calibration;
Robot robot(RobotPins(), calibration);

const Step route[] = {
    // Leave the entrance and reach the roundabout tangent point.
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::Forward, DRIVE_30_CM_MS, 0},
    {Action::TurnRight90, 0, 0},

    // Two complete laps. Adjust arcInnerSpeed/arcOuterSpeed in setup() so the
    // robot keeps the 10 cm centre marker on its right-hand side.
    {Action::ArcRight, ROUNDABOUT_LAP_MS, 0},
    {Action::ArcRight, ROUNDABOUT_LAP_MS, 0},

    // The first gate is now opened by the field lap counter.
    {Action::TurnRight90, 0, 0},
    {Action::WaitForGate, GATE_WAIT_TIMEOUT_MS, 32},
    {Action::ForwardUntilObstacle, 1800, 12},
    {Action::PressSwitch, SWITCH_PUSH_MS, 0},
    {Action::Backward, DRIVE_30_CM_MS / 2, 0},

    // Return to the centre lane and leave through the second gate.
    {Action::TurnAround, 0, 0},
    {Action::Forward, DRIVE_30_CM_MS, 0},
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
