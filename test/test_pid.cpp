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

    return 0;
}