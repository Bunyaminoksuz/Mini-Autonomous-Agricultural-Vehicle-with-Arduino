# Mini-Autonomous-Agricultural-Vehicle-with-Arduino
It is a Mini Autonomous Agricultural Vehicle project with Arduino.
Mini Autonomous Agricultural Vehicle

The Mini Autonomous Agricultural Vehicle is a vehicle that measures soil moisture in a designated agricultural area, detects obstacles with sensors while moving, and waters the soil if it is not moist. Our vehicle has an Arduino-based control system and will be able to move by detecting surrounding obstacles thanks to its sensors. With the soil moisture sensor on it, it will be able to measure the soil's moisture and water it accordingly. And it will move by avoiding obstacles on a designated route.

Materials and Tools Used

Hardware

Arduino Board
Used for tasks such as reading data from sensors, controlling motors, and interacting with other electronic components.

Distance Sensor
Sensors used to detect the distance of surrounding objects. They generally work using ultrasonic waves (like HC-SR04) or infrared rays. They are used in robotics projects for applications like obstacle avoidance, distance measurement, and motion detection.

Servo Motor
A type of motor that can precisely move to a specific angle and hold that position. They are generally used in robot arms, camera platforms, and mechanisms requiring precise control. They are controlled by PWM (Pulse Width Modulation) signals.

Soil Moisture Sensor
A sensor used to measure the moisture level within the soil. It is generally used in plant watering systems to determine how much water the soil needs. There are different types that work on the principle of resistance or capacitance.

Water Pump
A mechanical device used to transfer a liquid, water, from one place to another. They can be of different sizes and capacities. They are used in automatic watering systems or robotic projects requiring liquid transfer. They are generally driven by a DC motor.

DC Motor
An electric motor that runs on direct current (DC). They convert electrical energy into rotational motion. There are varieties with different speed and torque characteristics. They are used to turn robot wheels, operate pumps, or provide other mechanical movements.

Motor Driver
Electronic circuits used to control the speed, direction, and stopping of high-current and voltage-requiring motors, such as DC motors, with signals from low-current control devices like microcontrollers. Integrations like L298N are commonly used.

Li-Ion Battery
A rechargeable battery type with high energy density, long life, and lightness. They are widely used as a power source in robotic projects and portable electronic devices. They can have different voltage and capacity ratings.

4WD Car Chassis
The basic mechanical structure of a robot car platform with four-wheel drive. It generally includes motors, wheels, and the body. It provides mobility and the ability to travel on different terrains in robotic projects.

Libraries Used

Servo.h
The standard Arduino library used to control the servo motor, which raises and lowers the soil moisture sensor.

(Note: This project uses built-in Arduino functions like digitalWrite for motor control (with a driver like L298N) and pulseIn for the distance sensor, hence AFMotor.h and NewPing.h are not required.)

## Pin Connections

| Component | Arduino Pin(s) |
| :--- | :--- |
| **Motor Driver (L298N)** | |
| IN1 (Left Motor) | 6 |
| IN2 (Left Motor) | 7 |
| IN3 (Right Motor) | 8 |
| IN4 (Right Motor) | 9 |
| **Water Pump Relay** | 10 |
| **Soil Moisture Sensor** | A0 |
| **HC-SR04 Distance Sensor** | |
| TRIG Pin | 11 |
| ECHO Pin | 12 |
| **Servo Motor** | 5 |
