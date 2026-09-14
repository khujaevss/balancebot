from pid import PID
import pytest

def test_kp():
    pid = PID(2, 0, 0, 5)
    assert pid.compute(3, 1) == 4

def test_ki():
    pid = PID(0, 3, 0)
    assert pid.compute(10, 2) == -60
    assert pid.compute(10, 1) == -90


def test_kd():
    pid = PID(0, 0, 3)
    assert pid.compute(4, 2) == -6
    assert pid.compute(6, 0.5) == -12


def test_integral_limit():
    pid = PID(0, 2, 0, setpoint=1, integral_limit=1)
    assert pid.compute(0, 0.5) == pytest.approx(1)
    assert pid.compute(0, 0.5) == pytest.approx(2)
    assert pid.compute(0, 0.5) == pytest.approx(2)


def test_compute():

    pid = PID(1, 1, 0, integral_limit=None)
    assert pid.compute(1.0, 0.0106) == pytest.approx(-1.0106)





def test_reset():
    pid = PID(1, 1, 1)

    pid.compute(2, 0.5)

    assert pid.integral != 0.0
    assert pid.last_measurement != 0.0

    pid.reset()

    assert pid.integral == pytest.approx(0.0)
    assert pid.last_measurement == pytest.approx(0.0)