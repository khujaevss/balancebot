#pragma once

class PID
{
    private:
        float kp;
        float ki;
        float kd;
        float integral;
        float prevMeasurement;
        float integralMax;
        bool firstCall;
    public:
        PID (float p, float i, float d);

        float update(float measurement, float setpoint, float dt);

        void reset();

        void setGains(float p, float i, float d);

};