ESP32 Gesture-Controlled Robot
A low-latency, wireless robotics project utilizing the ESP-NOW protocol for seamless communication between a gesture-sensing glove and a robotic chassis.

System Architecture
This project consists of two independent ESP32 units:

Transmitter (The Glove): Uses an MPU6050 (Accelerometer/Gyroscope) to detect hand tilt. It maps these tilt values to directional commands ('F', 'B', 'L', 'R', 'S').

Receiver (The Car): An ESP32 that receives commands via ESP-NOW. It features dual-protocol support (Bluetooth and ESP-NOW) with a priority-management system to ensure smooth operation.

Features
ESP-NOW Protocol: Provides near-instantaneous wireless communication, significantly faster than standard Bluetooth.

Dual-Mode Control: The car can be controlled via the gesture glove (ESP-NOW) or a standard Bluetooth Serial app.

Safety Timeout: The receiver automatically stops the motors if the signal is lost for >1 second.

Priority Management: Bluetooth commands override ESP-NOW gestures to allow for emergency manual control.

Setup Instructions
Configure Receiver: Upload receiver.ino to the car's ESP32. Open the Serial Monitor to retrieve its MAC address.

Update Transmitter: Copy the MAC address from the receiver and update the receiverMAC[] array in transmitter.ino.

Upload Transmitter: Upload the modified transmitter.ino to the ESP32 mounted on your glove.

Calibration: If the robot moves erratically, check your MPU6050 serial output to fine-tune the tilt threshold values (7000 / 3000 in the loop).

Hardware Used
Microcontrollers: 2x ESP32 Development Boards

Sensors: MPU6050 (6-Axis Accelerometer/Gyro)

Motors: L298N Motor Driver + 2x DC Gear Motors

Power: 12V Battery Pack
