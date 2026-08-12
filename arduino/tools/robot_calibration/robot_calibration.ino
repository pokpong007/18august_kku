#include <DINCRobot.h>

using namespace dinc_robot;

RobotCalibration calibration;
Robot robot(RobotPins(), calibration);

void printMenu() {
  Serial.println();
  Serial.println(F("=== DINC ROBOT CALIBRATION ==="));
  Serial.println(F("วางหุ่นยนต์บนแท่นยกล้อก่อนทดสอบ"));
  Serial.println(F("พิมพ์คำสั่งใน Serial Monitor แล้วกด Send:"));
  Serial.println(F("  f = เดินหน้าตามเวลา 30 cm"));
  Serial.println(F("  b = ถอยหลังตามเวลา 30 cm"));
  Serial.println(F("  l = เลี้ยวซ้าย 90 องศา"));
  Serial.println(F("  r = เลี้ยวขวา 90 องศา"));
  Serial.println(F("  u = อ่าน ultrasonic 10 ครั้ง"));
  Serial.println(F("  s = อ่าน sensor ทั้งหมด 10 ครั้ง"));
  Serial.println(F("  1 = หันหัว ultrasonic ซ้ายและอ่านระยะ"));
  Serial.println(F("  2 = หันหัว ultrasonic ตรงและอ่านระยะ"));
  Serial.println(F("  3 = หันหัว ultrasonic ขวาและอ่านระยะ"));
  Serial.println(F("  x = หยุดมอเตอร์"));
  Serial.println(F("  h = แสดงเมนูนี้อีกครั้ง"));
  Serial.println();
  Serial.print(F("DRIVE_30_CM_MS = "));
  Serial.println(settings::DRIVE_30_CM_MS);
  Serial.print(F("TURN_LEFT_90_MS = "));
  Serial.println(settings::TURN_LEFT_90_MS);
  Serial.print(F("TURN_RIGHT_90_MS = "));
  Serial.println(settings::TURN_RIGHT_90_MS);
  Serial.print(F("HEAD_LEFT/CENTER/RIGHT = "));
  Serial.print(settings::HEAD_LEFT_ANGLE);
  Serial.print(F(" / "));
  Serial.print(settings::HEAD_CENTER_ANGLE);
  Serial.print(F(" / "));
  Serial.println(settings::HEAD_RIGHT_ANGLE);
}

void printSensorsOnce() {
  Serial.print(F("distance_cm="));
  Serial.print(robot.readDistanceCm());
  Serial.print(F(" | line_L="));
  Serial.print(robot.leftLineActive());
  Serial.print(F(" line_R="));
  Serial.print(robot.rightLineActive());
  Serial.print(F(" | IR_L="));
  Serial.print(robot.leftObstacleBlocked());
  Serial.print(F(" IR_R="));
  Serial.print(robot.rightObstacleBlocked());
  Serial.print(F(" | LDR_L="));
  Serial.print(robot.readLdrLeft());
  Serial.print(F(" LDR_R="));
  Serial.println(robot.readLdrRight());
}

void setup() {
  // ปิด safety เฉพาะสเก็ตช์ทดสอบ เพื่อให้ทดสอบล้อบนแท่นได้ง่าย
  calibration.enableEdgeSafety = false;
  calibration.enableObstacleStop = false;
  robot.setCalibration(calibration);
  robot.begin(115200);
  robot.stop();
  delay(400);
  printMenu();
}

void loop() {
  if (!Serial.available()) {
    return;
  }

  const char command = Serial.read();
  switch (command) {
    case 'f':
    case 'F':
      Serial.println(F("TEST: forward 30 cm"));
      robot.runTimed(calibration.driveSpeed, calibration.driveSpeed,
                     settings::DRIVE_30_CM_MS, true);
      break;

    case 'b':
    case 'B':
      Serial.println(F("TEST: backward 30 cm"));
      robot.runTimed(-calibration.driveSpeed, -calibration.driveSpeed,
                     settings::DRIVE_30_CM_MS, true);
      break;

    case 'l':
    case 'L': {
      Serial.println(F("TEST: turn left 90 degrees"));
      const Step step = {Action::TurnLeft90, 0, 0};
      robot.execute(step);
      break;
    }

    case 'r':
    case 'R': {
      Serial.println(F("TEST: turn right 90 degrees"));
      const Step step = {Action::TurnRight90, 0, 0};
      robot.execute(step);
      break;
    }

    case 'u':
    case 'U':
      for (uint8_t i = 0; i < 10; ++i) {
        Serial.print(F("distance_cm="));
        Serial.println(robot.readDistanceCm());
        delay(250);
      }
      break;

    case 's':
    case 'S':
      for (uint8_t i = 0; i < 10; ++i) {
        printSensorsOnce();
        delay(250);
      }
      break;

    case '1':
      Serial.print(F("HEAD LEFT distance_cm="));
      Serial.println(robot.scanLeftCm());
      break;

    case '2':
      Serial.print(F("HEAD CENTER distance_cm="));
      Serial.println(robot.scanCenterCm());
      break;

    case '3':
      Serial.print(F("HEAD RIGHT distance_cm="));
      Serial.println(robot.scanRightCm());
      break;

    case 'x':
    case 'X':
      robot.stop();
      Serial.println(F("STOP"));
      break;

    case 'h':
    case 'H':
      printMenu();
      break;

    case '\n':
    case '\r':
    case ' ':
      break;

    default:
      Serial.println(F("ไม่รู้จักคำสั่ง พิมพ์ h เพื่อดูเมนู"));
      break;
  }
}
