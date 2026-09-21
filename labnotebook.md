# BalanceBot Lab Notebook

---

## Day 2 — Gyro Drift Experiment

**What I did:**
I held the sensor still, in one exact place, and measured its drift rate — a built-in error every gyroscope has due to a small bias baked into the chip. The purpose of this experiment was to reveal how much the angle reading drifts even when the sensor isn't moving, since this error accumulates over time and becomes significant if left uncorrected


**What I observed:**

 0s   =  ~0°  

 30s  =  21°

 60s  =  43°

 120s =  87°

 Drift rate: roughly 0.7°/sec, consistent throughout. Sensor never moved.


**Why this happens:**
This happens because of a built-in bias in the gyroscope chip. I ran this experiment to confirm the drift exists and measure roughly how large it is. This drift will later be corrected — not by removing the exact bias, but by blending in accelerometer data (the complementary filter) that has no drift of its own.

**Why it matters:**
Without correcting for this drift, the error keeps adding up over time and becomes large enough to seriously disrupt balancing — and balancing is the core function of my robot.

---

## Day 2 — Accelerometer Noise & Vibration Experiment

**What I did:**
I held the sensor still, then observed the noise in the sensor through the Serial Plotter.

**What I observed:**
Part A (noise at rest):
The values stayed between 0 and 1.3 degrees and never climbed higher than that while the sensor was still.

Part B (tap test):
Spikes were observed while tapping. The spikes were positive when tapped on one side and negative when tapped on the other side, due to the direction of the extra force added on top of gravity.

**Why this happens:**
The noise at rest happens because of small electrical/mechanical 
imperfections in the chip — a built-in limitation. The tap 
spikes happen because tapping adds a real extra force on top of gravity, and the accelerometer can't tell the difference between "extra force from a tap" and "extra force from tilting."

**Why it matters:**
If the accelerometer were used alone to control the robot, every vibration (like motor vibration once the robot is moving) would show up as a fake tilt reading, causing the robot to react to noise instead of real tilting. This is the accelerometer's main weakness on its own.

---

## Day 2 — Conclusion: Why I Need a Complementary Filter

**What each sensor gets wrong:**
Gyro: its error adds up over time — even a tiny constant bias (0.7°/sec) keeps building forever, so left alone it eventually reports completely wrong angles, even if the sensor never moved.

Accelerometer: it reacts to any force, not just tilt — a small tap causes a real spike in the reading, making it too jumpy to trust on its own.

**What each sensor gets right:**
Gyro: it's smooth and doesn't react to taps or vibration, since it measures rotation speed, not force.

Accelerometer: it doesn't build up error over time — every reading is fresh, based on gravity right now, with no memory of past readings, so it never drifts.

**Why fusion has to happen in software:**
This is because we only get raw data about accel and gyro from the sensor - the sensor doesn't calculate the angle itself. That's what the code is for: using math to combine the two into one corrected angle.

---

## Day 3 — Complementary Filter Implementation

**What I did:**
I kept the sensor still and observed the data through Serial Plotter, then tapped the table and observed how both lines reacted.

**What I observed:**
While still, the complementary filter line stayed close to ~1.2°, while the raw accelerometer line kept bouncing up and down.

During the tap test, the complementary filter line showed almost no reaction, while the raw accelerometer line showed large, unfavorable spikes.

**Why this happens:**
This happens because the filter is weighted 95% toward the gyro (which is smooth and doesn't react to vibration) and only 5% toward the accelerometer (which is noisy). A tap makes the raw accelerometer spike, but that spike only gets a 5% vote in the final filtered angle — barely visible in the output.

**Why it matters:**
This matters because a real robot will constantly experience small 
vibrations and bumps (from its own motors, an uneven floor, etc). If it reacted to every one of these as if it were a real tilt, it would overcorrect constantly and likely fall. The filter lets the robot ignore these small imperfections while still reacting correctly to real tilting.

**Extra finding — startup initialization:**
If the sensor starts already tilted (say at 90°) and `complementaryAngle` is initialized to 0, the filter would slowly climb from 0 toward 90° over roughly a second, instead of immediately showing the correct angle — because it only gets a small 5% nudge from `accelAngle` each loop. I fixed this by initializing `complementaryAngle` directly from `accelAngle` on the very 
first loop, so the filter starts at the correct angle immediately instead of climbing up to it.


## 6 Sep 2026 — IMU calibration, loop timing, alpha

**Calibration** — ran `IMU_Zero`, hardcoded offsets in `setup()`:
accel -2178 / 164 / 1600 · gyro -78 / -10 / -25
- Gyro drift: 0.7 → 0.0268 °/s (84° → 3.2° over 2 min)
- Offsets fixed at compile time, not re-measured at boot

**Output** — now prints accel / gyro / fused, comma-separated for plotting.
Previously only fused, which hid both failure modes.

**Timing** — `delay(10)` → `micros()` wait. I2C 400 kHz, baud 115200.
- Measured 470 loops / 5 s = **94 Hz**, dt = 0.0106 s
- Serial prints made no difference to the count

**Alpha** — α = τ/(τ+dt), τ = 0.3, dt = 0.0106 → **0.9659 / 0.0341**
- Old 0.95 already equalled τ = 0.20 s, so this was 0.20 → 0.30 s
- Little visible change. Now derived rather than guessed

**Notes**
- Tap test: accel spikes (shock misread as gravity), gyro flat
- Accel hit -178° once — `atan2` wrapping at ±180, will recur on a real fall


## 11 Sep 2026 — PID class

**Theory**

Watched the three MATLAB PID videos and wrote answers to six concept
questions: error sign convention, Kp too high vs too low, steady-state
offset, why dt belongs in the integral, Kd on a noisy sensor, and windup.

**Python**

CS50P week 8 (OOP) and the Cookie Jar problem set, check50 passing.

**PID class — `python/pid.py`**

Built in five passes, each tested before starting the next: proportional,
integral, integral clamp, derivative, `reset()`.

Three decisions:

- Integral clamped in both directions, silently — no exception, since it
  engages whenever the robot is held off the ground
- Derivative taken on the measurement, not the error, to avoid a spike
  when the outer loop starts moving the setpoint
- Previous measurement stored *after* the derivative is calculated —
  reversing those two lines makes the derivative read zero on every call

**Bugs**

- Lower clamp branch assigned the positive limit instead of the negative
- The ordering issue above

Neither raised an error at runtime.

**Status**

All five checks pass. Converting them to pytest on Sunday.

**Outstanding**

First call after `reset()` spikes the derivative — the stored previous measurement starts at zero while the robot sits at a real angle. Week 3.

## 15–20 Sept 2026 — C++ revision, PID ported to C++, loop pinned at 100 Hz

### C++ revision
- Worked through Bro Code topics 1–20 (basics → inheritance). Drilled namespaces,
  pointers, structs, classes with small exercises (`Battery`, `RateLimiter`).
- Kept tripping on the same thing: putting a type in front of an assignment
  (`int charge = ...`, `float kp = ...`) inside a method. Creates a local that
  shadows the member and dies at the end of the function. Member never changes.
- Ran a .cpp with Python by accident — Python's SyntaxError is the giveaway.

### PID class in C++ (`firmware/balance/PID.h`, `PID.cpp`)
- Ported from the Python version. Gains + integral + prevMeasurement + firstCall.
- Anti-windup: integral clamped at 255/ki, so the I term alone can't exceed full
  PWM. Guarded for ki = 0 (255/0 → inf → clamp silently never fires).
  Started with a magic 50 — wrong, it only means anything for one ki.
- Derivative on measurement, not error → no derivative kick when the setpoint
  jumps. Matters once the Pi sends lean targets.
- firstCall flag skips D on the first update. Without it, starting at 30° with
  prevMeasurement = 0 fires ~3000 × kd at the motors. reset() re-arms it.
- Bugs found: `prevMeasurement = measurement` placed before the D calc → D always 0;
  `if (dt = 0)` assigned instead of compared (-Wall caught it).
- `test/test_pid.cpp`: 7 checks (P, I, D, clamp, reset, first-call, reset re-arm).
  All pass. Rerun after any change to PID.cpp.

### Loop timing: 94 → 100 Hz
- Old loop waited 10 ms from *after* the IMU read → 10 ms + ~0.6 ms → 94 Hz.
- Now on a fixed timetable: `lastTick += 10000`. Lateness doesn't accumulate.
  Unsigned subtraction survives the micros() wrap.
- Confirmed on hardware: LOOPS: 500 per 5 s.
- dt changed 0.0106 → 0.0100, so α = 0.9659 now gives τ ≈ 0.283 s (was 0.300 s).
  α-from-dt fix still open.

### PID on real IMU data (Kp 10, Ki 0, Kd 0, balancePoint 0)
- Flat: angle ≈ 0, output jitters ±0.3 → ±0.03° of angle noise.
- ±5° tilt → ±50. Matches 10 × 5.
- **Sign:** VCC edge down → angle −5°, output +50. INT edge down → angle +5°, output −50.

### Open
- Fall cutoff (>45° → output 0 + reset), output clamp ±255, α-from-dt
- driveMotors() for DRV8833, log_serial.py
- Order parts if not already. Real balancePoint needs the chassis.



