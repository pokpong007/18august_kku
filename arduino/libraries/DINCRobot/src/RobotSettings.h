#ifndef DINC_ROBOT_SETTINGS_H
#define DINC_ROBOT_SETTINGS_H

#include <Arduino.h>

// แก้ค่าคาลิเบรตของหุ่นยนต์ที่ไฟล์นี้ไฟล์เดียว
// สเก็ตช์หุ่นยนต์ทั้ง 4 สนามและสเก็ตช์ทดสอบจะใช้ค่าชุดนี้ร่วมกัน
namespace dinc_robot {
namespace settings {

// ความเร็วมอเตอร์: 0-255
constexpr int16_t DRIVE_SPEED = 155;
constexpr int16_t SLOW_SPEED = 105;
constexpr int16_t TURN_SPEED = 145;

// เวลาเคลื่อนที่ที่ต้องวัดจากหุ่นยนต์จริง
constexpr uint16_t DRIVE_30_CM_MS = 1120;
constexpr uint16_t TURN_LEFT_90_MS = 530;
constexpr uint16_t TURN_RIGHT_90_MS = 530;
constexpr uint16_t TURN_180_MS = 1060;

// ทิศมอเตอร์ ถ้าสั่งเดินหน้าแล้วล้อข้างใดกลับทาง ให้สลับ true/false ข้างนั้น
constexpr bool LEFT_MOTOR_FORWARD_HIGH = true;
constexpr bool RIGHT_MOTOR_FORWARD_HIGH = false;

// มุม servo ที่หมุนหัว ultrasonic (D10)
// ทดสอบด้วยคำสั่ง 1/2/3 ใน robot_calibration แล้วค่อยแก้ตามรถจริง
constexpr uint8_t HEAD_LEFT_ANGLE = 150;
constexpr uint8_t HEAD_CENTER_ANGLE = 90;
constexpr uint8_t HEAD_RIGHT_ANGLE = 30;
constexpr uint16_t HEAD_SETTLE_MS = 300;

// ค่าวงเวียนสนาม 1
constexpr int16_t ROUNDABOUT_INNER_SPEED = 88;
constexpr int16_t ROUNDABOUT_OUTER_SPEED = 174;
constexpr uint16_t ROUNDABOUT_ONE_LAP_MS = 3900;

}  // namespace settings
}  // namespace dinc_robot

#endif
