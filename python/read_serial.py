import serial
ser = serial.Serial('/dev/ttyACM0', 38400, timeout=1)

while True:
    cleanser=ser.readline().decode('utf-8').strip()
    print(cleanser)