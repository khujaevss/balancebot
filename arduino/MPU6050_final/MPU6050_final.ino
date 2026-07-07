#include <MPU6050.h>
#include <Wire.h>

MPU6050 mpu;
float gyroAngle = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(38400);
  Wire.begin();
  mpu.initialize();
  lastTime = micros();
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float gyroRate = gz / 131.0;
  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  gyroAngle += gyroRate * dt;
  Serial.println(gyroAngle);
  delay(10);
}