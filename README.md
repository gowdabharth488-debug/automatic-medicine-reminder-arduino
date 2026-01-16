# IoT-Based Automated Medicine Reminder Box for Aged Persons

## 📌 Project Overview
This project is an IoT-based automated medicine reminder system designed to assist elderly and chronic patients in taking medicines on time. The system uses Arduino Mega, RTC, IR sensors, GSM module, and LCD to provide real-time alerts and caregiver notifications.

The system alerts the patient using buzzer and LEDs and verifies medicine intake using IR sensors. If medicine is not taken within a specified time, an SMS and phone call alert is sent to the caregiver using a GSM module.

---

## 🎯 Objectives
- Ensure timely medicine intake
- Reduce dependency on caretakers
- Minimize missed medication doses
- Provide remote alerts to caregivers

---

## 🚀 Features
- Multiple medicine reminder timings
- Real-Time Clock (DS3231) with battery backup
- IR sensors to detect medicine removal
- GSM-based SMS and call alerts
- 16x2 LCD display for system status
- Buzzer and LED alerts
- EEPROM memory for data retention

---

## 🛠️ Hardware Components
- Arduino Mega 2560 (ATmega2560)
- RTC Module DS3231
- GSM Module (Quectel EC200U)
- IR Transmitter & Receiver pairs
- 16x2 LCD Display
- Buzzer
- LEDs with resistors
- Push buttons
- EEPROM

---

## ⚙️ Software Used
- Arduino IDE
- Embedded C/C++

---

## 🧠 Working Principle
1. User sets medicine reminder time using push buttons
2. RTC continuously tracks current time
3. At scheduled time:
   - Buzzer sounds
   - LED glows
4. IR sensor checks if medicine is taken
5. If not taken:
   - SMS and call sent via GSM module
6. All settings are stored in EEPROM


