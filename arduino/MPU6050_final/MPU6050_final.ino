#include <MPU6050.h>
#include <Wire.h>

MPU6050 mpu;

void setup() {
  Serial.begin(38400);
  Wire.begin();
  mpu.initialize();
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float accelAngle = atan2((float)ay, (float)az) * 180.0 / PI;

  Serial.println(accelAngle);
  delay(50);
}