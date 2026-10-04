#include <MPU6050.h>
#include <Wire.h>
#include "PID.h"

MPU6050 mpu;

float complementaryAngle = 0.0;
float gyro = 0.0;
bool firstReading = true;

unsigned long lastTime = 0;      // for dt
unsigned long lastTick = 0;      // NEW: start of the current 10 ms slot
unsigned long timElapsed = 0;    // for the loop counter
unsigned long n = 0;

const unsigned long LOOP_US = 10000;   // NEW: 100 Hz = 10 000 µs

PID pid(10, 0, 0);
float balancePoint = 0;
bool motorsEnabled = false;
int uprightCount = 0;

const float TRIP_ANGLE = 45.0;   // if past this then disarm
const float ARM_ANGLE  = 5.0;    // must be inside this to arm
const int   ARM_LOOPS  = 200;    // wait 2s before balancing


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
  lastTick = micros();           // NEW
  timElapsed = micros();
}

void loop()
{
  // NEW: wait for the next 10 ms slot, BEFORE doing any work
  while (micros() - lastTick < LOOP_US) { }
  lastTick += LOOP_US;

  // dt now measured at the start of each slot
  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  n = n + 1;

  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float accelAngle = atan2((float)ay, (float)az) * 180.0 / PI;
  float gyroRate = gx / 131.0;
  gyro = gyro + gyroRate * dt;

  if (now - timElapsed >= 5000000)
  {
    Serial.print("LOOPS: ");     // re-enabled: should say 500
    Serial.println(n);
    n = 0;
    timElapsed = now;
  }

  if (firstReading) 
  {
    complementaryAngle = accelAngle;
    firstReading = false;
  } 
  
  else 

  {
    complementaryAngle = 0.9677 * (complementaryAngle + gyroRate * dt) + 0.0323 * accelAngle;   // α = τ/(τ+dt)
  }

  if (motorsEnabled == true && fabs(complementaryAngle) > TRIP_ANGLE)
  {
    motorsEnabled = false;
    uprightCount = 0;
    pid.reset();
  }

  if (motorsEnabled == false)
  {
    if (fabs(complementaryAngle) < ARM_ANGLE)
      {
        uprightCount += 1;
      }
    else
      {
        uprightCount=0;
      }
    if (uprightCount>= ARM_LOOPS)
      {
        motorsEnabled = true;
      }
  }

  float output = 0;

  if (motorsEnabled == true)
  {
    output = pid.update(complementaryAngle, balancePoint, dt);
  }

  Serial.print(accelAngle);
  Serial.print(",");
  Serial.print(gyro);
  Serial.print(",");
  Serial.print(complementaryAngle);
  Serial.print(",");
  Serial.print(output);
  Serial.print(",");
  Serial.print(motorsEnabled);
  Serial.print(",");
  Serial.print(uprightCount);
  Serial.println();
}