#include <MPU6050.h>
#include <Wire.h>

MPU6050 mpu;

float complementaryAngle = 0.0;
float gyro = 0.00;
unsigned long lastTime = 0;
bool firstReading = true;
unsigned long timElapsed = 0;


void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  mpu.initialize();
  mpu.setXAccelOffset(-2178);
  mpu.setYAccelOffset(164);
  mpu.setZAccelOffset(1600);
  mpu.setXGyroOffset(-78);
  mpu.setYGyroOffset(-10);
  mpu.setZGyroOffset(-25);
  lastTime = micros();
  timElapsed = micros();

}

unsigned long n = 0;

void loop() 

{
  n = n+1;


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
  gyro = gyro + gyroRate * dt;
  
  if (micros()-timElapsed >= 5000000)
  {
    //Serial.print("LOOPS:                              ");
    //Serial.println(n);
    n=0;
    timElapsed = now;
  }



  if (firstReading) {
    // Startup fix: initialize from accelAngle instead of 0
    // (avoids slow climb from 0 to the real starting angle)
    complementaryAngle = accelAngle;
    firstReading = false;
  } else {
    // Complementary filter — fuse both
    complementaryAngle = 0.9659 * (complementaryAngle + gyroRate * dt) + 0.0341 * accelAngle;
  }

  // Print both for comparison

  Serial.print(accelAngle);
  Serial.print(",");
  Serial.print(gyro);
  Serial.print(",");
  Serial.print(complementaryAngle);
  Serial.println();

  while (micros() - now < 10000) 
{
  
  
}




  
}