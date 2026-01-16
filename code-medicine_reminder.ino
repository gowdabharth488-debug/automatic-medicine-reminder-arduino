#include <LiquidCrystal.h>
#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>

// LCD pins: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

RTC_DS3231 rtc;

// Push Button pins
const int button1Pin = 22; // Set hour
const int button2Pin = 23; // Set minute
const int button3Pin = 24; // Set next dose
const int button4Pin = 25; // Save doses

// LED pins
const int ledPins[4] = {6, 7, 8, 9};

// IR Receiver pins
const int irPins[4] = {A0, A1, A2, A3};

// Buzzer pin
const int buzzerPin = 10;

// Variables for medicine times (hours and minutes)
int doseHours[4];
int doseMinutes[4];

// Medicine taken status
bool doseTaken[4] = {false, false, false, false};

// For setting doses
int selectedDose = 0;
bool isSetting = false;  // Flag to indicate if we're setting the dose

// Timer variables for non-blocking logic
unsigned long doseTimer[4] = {0, 0, 0, 0};
const unsigned long doseWindow = 60000;  // 1 minute window for checking medicine taken

// Function prototype for displayAndSend
void displayAndSend(String line1, String line2 = "");

void setup() {
  Serial.begin(9600);    // PC
  Serial1.begin(9600);   // HC-05

  lcd.begin(16, 2);
  Wire.begin();

  if (!rtc.begin()) {
    lcd.print("RTC not found!");
    while (1);
  }

  if (rtc.lostPower()) {
    lcd.clear();
    lcd.print("RTC Resetting");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(2000);
    lcd.clear();
  }

  for (int i = 0; i < 4; i++) {
    pinMode(button1Pin + i, INPUT_PULLUP);
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  pinMode(buzzerPin, OUTPUT);

  for (int i = 0; i < 4; i++) {
    doseHours[i] = EEPROM.read(i * 2);
    doseMinutes[i] = EEPROM.read(i * 2 + 1);
  }

  displayAndSend("System Ready!");
  delay(2000);
  lcd.clear();
}

void loop() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("Time:");
  lcd.print(twoDigits(now.hour()));
  lcd.print(":");
  lcd.print(twoDigits(now.minute()));
  lcd.print(":");
  lcd.print(twoDigits(now.second()));  // Add seconds

  // Handle buttons
  if (digitalRead(button1Pin) == LOW) { // Hour ++
    doseHours[selectedDose] = (doseHours[selectedDose] + 1) % 24;
    showDoseSet(selectedDose);
    isSetting = true;  // Set flag when adjusting time
    delay(300);
  }

  if (digitalRead(button2Pin) == LOW) { // Minute ++
    doseMinutes[selectedDose] = (doseMinutes[selectedDose] + 1) % 60;
    showDoseSet(selectedDose);
    isSetting = true;  // Set flag when adjusting time
    delay(300);
  }

  if (digitalRead(button3Pin) == LOW) { // Next dose
    selectedDose = (selectedDose + 1) % 4;
    showDoseSet(selectedDose);
    isSetting = true;  // Set flag when changing dose
    delay(300);
  }

  if (digitalRead(button4Pin) == LOW) { // Save doses
    for (int i = 0; i < 4; i++) {
      EEPROM.write(i * 2, doseHours[i]);
      EEPROM.write(i * 2 + 1, doseMinutes[i]);
    }
    displayAndSend("Dose Times Saved");
    isSetting = false;  // Reset flag after saving
    delay(1000);
    lcd.clear();
  }

  // Only check for dose time if we are not setting it
  if (!isSetting) {
    // Check if it's time for any dose
    for (int i = 0; i < 4; i++) {
      if (now.hour() == doseHours[i] && now.minute() == doseMinutes[i] && !doseTaken[i]) {
        // Check if the 1-minute window has passed since the last check
        if (millis() - doseTimer[i] >= doseWindow) {
          handleDose(i);
          doseTimer[i] = millis(); // Reset the timer for the next check
        }
      }
    }
  }

  delay(300);  // Small delay to keep the program responsive
}

void handleDose(int doseNum) {
  // Buzzer 4 times
  for (int i = 0; i < 4; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);
    delay(200);
  }

  // LED ON
  digitalWrite(ledPins[doseNum], HIGH);

  unsigned long startMillis = millis();
  bool medicineTaken = false;

  // Non-blocking check for IR sensor, over 1-minute period
  while (millis() - startMillis < doseWindow) {
    int irVal = analogRead(irPins[doseNum]);
    if (irVal < 800) { // IR unblocked = medicine taken
      medicineTaken = true;
    }
    delay(50); // Small delay to allow other tasks to run
  }

  digitalWrite(ledPins[doseNum], LOW);

  if (medicineTaken) {
    doseTaken[doseNum] = true;
    displayAndSend("Dose ", String(doseNum + 1) + " Taken");
  } else {
    displayAndSend("Dose ", String(doseNum + 1) + " Not Taken");
  }

  delay(3000);
  lcd.clear();
}

void showDoseSet(int doseNum) {
  lcd.clear();
  lcd.print("Set Dose ");
  lcd.print(doseNum + 1);
  lcd.setCursor(0, 1);
  lcd.print(twoDigits(doseHours[doseNum]));
  lcd.print(":");
  lcd.print(twoDigits(doseMinutes[doseNum]));
}

void displayAndSend(String line1, String line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  if (line2 != "") {
    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
  Serial1.println(line1);
  if (line2 != "") Serial1.println(line2);
}

String twoDigits(int number) {
  if (number < 10) return "0" + String(number);
  else return String(number);
}
