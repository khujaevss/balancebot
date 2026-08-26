import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64
import serial

class TiltPublisher(Node):
    def __init__(self):
        super().__init__('tilt_publisher')
        self.publisher = self.create_publisher(Float64, '/tilt_angle', 10)
        self.serial_port = serial.Serial('/dev/ttyACM0', 38400, timeout=1)
        self.timer = self.create_timer(0.01, self.read_and_publish)

    def read_and_publish(self):
        line = self.serial_port.readline().decode('utf-8').strip()
        if line:
            try:
                angle = float(line)
                msg = Float64()
                msg.data = angle
                self.publisher.publish(msg)
                print(f"Publishing: {angle}")
            except ValueError:
                pass
            
def main():
    rclpy.init()
    node = TiltPublisher()
    rclpy.spin(node)

main()