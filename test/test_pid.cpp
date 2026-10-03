#include <iostream>
#include "PID.h"

// Each block isolates one term by zeroing the other two gains.
// Run after any change to PID.cpp — all seven lines should match.

int main()
{
    // Pure P. Error of 10 with kp 5 is just 50, nothing clever.
    PID prop(5, 0, 0);
    std::cout << "P    " << prop.update(0, 10, 0.01) << "\t\t\texpect 50\n";

    // Pure I. Same error every call, so it should creep up in equal steps.
    // One object, three calls — that's the whole point, it has to remember.
    PID integ(0, 2, 0);
    std::cout << "I    " << integ.update(0, 10, 0.01) << " ";
    std::cout          << integ.update(0, 10, 0.01) << " ";
    std::cout          << integ.update(0, 10, 0.01) << "\t\texpect 0.2 0.4 0.6\n";

    // Pure D, on measurement not error. Still, then a 1-unit jump, then still.
    // Only the jump should produce anything.
    PID deriv(0, 0, 1);
    std::cout << "D    " << deriv.update(0, 0, 0.01) << " ";
    std::cout           << deriv.update(1, 0, 0.01) << " ";
    std::cout           << deriv.update(1, 0, 0.01) << "\t\texpect 0 -100 0\n";

    // Robot stuck in the air: error never goes away, integral would run forever.
    // Should climb, hit the ceiling, and sit there.
    PID wound(0, 5, 0);
    std::cout << "C   ";
    for (int i = 0; i < 8; i++)
    {
        std::cout << " " << wound.update(0, 100, 0.1);
    }
    std::cout << "\texpect 50 100 150 200 250 then flat\n";

    // Same wind-up, but picked up and reset. Should behave like a fresh object.
    PID cleared(0, 5, 0);
    for (int i = 0; i < 8; i++)
    {
        cleared.update(0, 100, 0.1);
    }
    cleared.reset();
    std::cout << "R    " << cleared.update(0, 100, 0.1) << "\t\t\texpect 50\n";

    // Startup: robot is already leaning 30 deg before the loop ever runs.
    // prevMeasurement is 0, so without the firstCall flag this fires 3000 at the motors.
    PID startup(0, 0, 1);
    std::cout << "F    " << startup.update(30, 0, 0.01) << " ";
    std::cout           << startup.update(30, 0, 0.01) << " ";
    std::cout           << startup.update(31, 0, 0.01) << "\t\texpect 0 0 -100\n";

    // reset() has to re-arm that flag too, or the jump to 50 kicks just as hard.
    startup.reset();
    std::cout << "FR   " << startup.update(50, 0, 0.01) << "\t\t\texpect 0\n";
        // Output clamp. kp 10, error 100 -> P = 1000, motor only takes 255.
    PID clampHi(10, 0, 0);
    std::cout << "OH   " << clampHi.update(0, 100, 0.01) << "\t\t\texpect 255\n";

    // Same, other direction.
    PID clampLo(10, 0, 0);
    std::cout << "OL   " << clampLo.update(0, -100, 0.01) << "\t\t\texpect -255\n";

    // Inside the limits: clamp must leave it alone.
    PID inside(10, 0, 0);
    std::cout << "ON   " << inside.update(0, 10, 0.01) << "\t\t\texpect 100\n";

    // Custom limits, like the 60/255 first motor spin.
    PID custom(10, 0, 0);
    custom.setOutputLimits(-60, 60);
    std::cout << "OC   " << custom.update(0, 100, 0.01) << "\t\t\texpect 60\n";

    // Limits in the wrong order must be ignored -> default 255 stays.
    PID badLim(10, 0, 0);
    badLim.setOutputLimits(60, -60);
    std::cout << "OB   " << badLim.update(0, 100, 0.01) << "\t\t\texpect 255\n";
    
        // Anti-windup. Maxed out for 20 steps (error +100), then a small error the other way.
    // Without anti-windup the stored integral keeps pushing the wrong way for 32 steps.
    PID windup(10, 5, 0);
    for (int i = 0; i < 20; i++)
    {
        windup.update(0, 100, 0.1);
    }
    int steps = 0;
    float out = 1;
    while (out >= 0 && steps < 100)
    {
        out = windup.update(0, -10, 0.1);
        steps++;
    }
    std::cout << "W    " << steps << "\t\t\texpect 1 (32 without anti-windup)\n";
    
    return 0;
}