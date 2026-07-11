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




