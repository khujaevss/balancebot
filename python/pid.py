class PID:
    def __init__ (self,  kp, ki, kd, setpoint = 0.0, integral_limit = None):
    
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.setpoint = setpoint
        self.integral = 0.0
        self.last_measurement = 0.0
        self.integral_limit = integral_limit


    def compute(self, mpu_reading, dt):

        error = self.setpoint - mpu_reading
        self.integral += error *dt
        if self.integral_limit is not None:
            if self.integral > self.integral_limit:
                self.integral = self.integral_limit
            elif self.integral < -self.integral_limit:
                self.integral = -self.integral_limit
        derivative = -(mpu_reading - self.last_measurement) / dt
        self.last_measurement = mpu_reading
        return self.kp * error + self.ki * self.integral + self.kd * derivative
    
    def reset(self):
        self.integral = 0.0
        self.last_measurement = 0.0