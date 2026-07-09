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