#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "GravityTDS.h"

// Define constants
const int PH_SENSOR_PIN = A0;
const int TURBIDITY_SENSOR_PIN = A1;
const int TDS_SENSOR_PIN = A2;
const int NUM_SAMPLES = 10;
const int NUM_TURBIDITY_SAMPLES = 800;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;

// Create LCD object
LiquidCrystal_I2C lcd(0x27, LCD_COLS, LCD_ROWS);

// Function declarations
float calculatePhValue();
float calculateTurbidityValue();
float roundToDecimal(float value, int decimalPlaces);

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize LCD display
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pH:");
  lcd.setCursor(8, 0);
  lcd.print("T:");
  lcd.setCursor(0, 1);
  lcd.print("TBD:");
}

void loop() {
  // Calculate pH value
  float phValue = calculatePhValue();

  // Print pH value to LCD display
  lcd.setCursor(3, 0);
  lcd.print(phValue);

  //TDS
  float tdsValue = calculateTDSValue();

  // Print TDS value to serial monitor
  lcd.setCursor(10, 0);
  lcd.print(tdsValue);

  // Calculate turbidity value
  float turbidityValue = calculateTurbidityValue();

  // Print turbidity value to LCD display
  lcd.setCursor(5, 1);
  lcd.print(turbidityValue);
  lcd.print(" NTU");
  

  // Delay for a short period before taking more readings
  delay(10);
}

float calculatePhValue() {
  // Read analog values from pH sensor
  int bufferArray[NUM_SAMPLES];
  for (int i = 0; i < NUM_SAMPLES; i++) {
    bufferArray[i] = analogRead(PH_SENSOR_PIN);
    delay(30);
  }
  // Sort analog values from small to large
  for (int i = 0; i < NUM_SAMPLES - 1; i++) {
    for (int j = i + 1; j < NUM_SAMPLES; j++) {
      if (bufferArray[i] > bufferArray[j]) {
        int temp = bufferArray[i];
        bufferArray[i] = bufferArray[j];
        bufferArray[j] = temp;
      }
    }
  }

  // Calculate average value of center samples
  unsigned long int avgValue = 0;
  for (int i = 2; i < NUM_SAMPLES - 2; i++) {
    avgValue += bufferArray[i];
  }
  float voltage = (float)avgValue * 5.0 / 1024 / (NUM_SAMPLES - 4);

  // Convert voltage to pH value
  float phValue = 3.5 * voltage;

  return phValue;
}

float calculateTurbidityValue() {
  // Read analog values from turbidity sensor
  float voltage = 0.0;
  for (int i = 0; i < NUM_TURBIDITY_SAMPLES; i++) {
    voltage += ((float)analogRead(TURBIDITY_SENSOR_PIN) / 1023) * 5;
  }
  voltage = voltage / NUM_TURBIDITY_SAMPLES;

  // Convert voltage to NTU value
  float turbidityValue = 0.0;
  if (voltage < 2.5) {
    turbidityValue = 3000.0;
  } else {
    turbidityValue = -1120.4 * pow(voltage, 2) + 5742.3 * voltage - 4353.8;
  }

  // Round turbidity value to two decimal places
  turbidityValue = roundToDecimal(turbidityValue, 2);

  return turbidityValue;
}
float calculateTDSValue() {
  int analogValue = analogRead(TDS_SENSOR_PIN);
  
  // Assuming you have a calibration formula for your specific TDS sensor
  // Replace these values with your own calibration coefficients
  float calibrationFactor = 0.5; // Example calibration factor
  float offset = 100;            // Example offset
  
  // Calculate TDS value using the calibration formula
  float tdsValue = (analogValue * calibrationFactor) + offset;
  
  return tdsValue;
}

float roundToDecimal(float value, int decimalPlaces) {
  float multiplier = pow(10.0f, decimalPlaces);
  float roundedValue = roundf(value * multiplier) / multiplier;
  return roundedValue;
}