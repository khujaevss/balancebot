#include <MPU6050.h>
#include <Wire.h>

MPU6050 mpu;

float complementaryAngle = 0.0;
unsigned long lastTime = 0;
bool firstReading = true;

void setup() {
  Serial.begin(38400);
  Wire.begin();
  mpu.initialize();
  lastTime = micros();
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Calculate dt in seconds
  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  // Accelerometer angle (noisy but no drift)
  float accelAngle = atan2((float)ay, (float)az) * 180.0 / PI;

  // Gyroscope rate in degrees/second
  float gyroRate = gx / 131.0;

  if (firstReading) {
    // Startup fix: initialize from accelAngle instead of 0
    // (avoids slow climb from 0 to the real starting angle)
    complementaryAngle = accelAngle;
    firstReading = false;
  } else {
    // Complementary filter — fuse both
    complementaryAngle = 0.95 * (complementaryAngle + gyroRate * dt) + 0.05 * accelAngle;
  }

  // Print both for comparison
  Serial.print(complementaryAngle);
  Serial.println();

  delay(10);
}