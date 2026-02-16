# Samrt--parking-system

## Overview

This project implements a Smart Parking System designed to monitor parking availability, control the gate automatically, and manage lighting efficiently.

The system uses multiple sensors and an STM32 microcontroller to automate parking operations and display available slots.

---

## Features

- Detect available parking slots using IR sensors
- Display available slots on LCD
- Automatic gate control using Ultrasonic sensors
- Automatic lighting control using IR sensors
- Real-time monitoring

---

## Hardware Components

- STM32 Microcontroller
- IR Sensors (5 total)
- Ultrasonic Sensors (2)
- LCD Display (I2C)
- Servo Motor
- LEDs
- Power Supply

---

## Software Tools

- Arduino IDE
- Embedded C Language

---

## PCB Design

The PCB was designed using Easy EDA.  
However, the PCB was not fabricated. The system was implemented and tested using prototype connections.

---

## How it Works

- IR sensors count available parking slots
- LCD displays number of free spaces
- Ultrasonic sensors detect cars approaching the gate
- Servo motor opens/closes gate automatically
- Lighting turns ON when car detected and OFF when no car present


---

## Author

Mohamed abdelnabi 
Electrical Engineering Student
