
#include "PID.h"


PID::PID(float p, float i, float d)
{
    kp = p;
    ki = i;
    kd = d;
    integral = 0;
    prevMeasurement = 0;
    firstCall = true;

    if (ki == 0)
    {
        integralMax = 0;
    }
    
    else
    {
        integralMax = 255 /ki;
    }
    
}

float PID::update(float measurement, float setpoint, float dt)
{
    float error = setpoint - measurement;

    float derivative;

    if (firstCall == true)
    {
        derivative = 0;
        firstCall = false;
    }
    else 
    {
        if (dt == 0)
        {
            derivative = 0;
        }
        else
        {
            derivative = -kd * (measurement - prevMeasurement) / dt;
        }

    }

    float wouldBe = kp*error + ki*integral + derivative;

    bool saturated = false;

     if (wouldBe > outMax && error > 0 || wouldBe < outMin && error < 0)
    {
        saturated = true;
    }   

    if (!saturated)
    {

        integral += error * dt;

        if (integral > integralMax)
        {
            integral = integralMax;
    
        }
        else if (integral < -integralMax)
        {
            integral = -integralMax;
        }

    }


    prevMeasurement = measurement;
    float output = kp*error + ki*integral + derivative;

    if (output > outMax)
    {
        output = outMax;
    }

    else if (output < outMin)
    {
        output = outMin;
    }

    return output;
}

void PID::reset()
{
    integral = 0;
    prevMeasurement = 0;
    firstCall = true;

}

void PID :: setGains( float p, float i, float d)
{
    kp = p;
    ki = i;
    kd = d;
    if (ki == 0)
    {
        integralMax = 0;
    }
    
    else
    {
        integralMax = 255 /ki;
    }
    
}

void PID::setOutputLimits( float min, float max)
{
    if (min < max)
    {
        outMin = min;
        outMax = max;
    }

}