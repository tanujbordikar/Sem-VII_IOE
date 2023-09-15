#include <Wire.h>
#include "GravityTDS.h"
#include<SoftwareSerial.h>
SoftwareSerial SUART(2, 3);

const int TDS_SENSOR_PIN = A0;
const int TURBIDITY_SENSOR_PIN = A1;
const int PH_SENSOR_PIN = A2;
const int NUM_SAMPLES = 10;
const int NUM_TURBIDITY_SAMPLES = 800;

void setup() {
  Serial.begin(115200);
  SUART.begin(115200);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
}

void loop() {
  float phValue = calculatePhValue();
  // Serial.println(phValue);

  float tdsValue = calculateTDSValue();
  // Serial.println(tdsValue);

  float turbidityValue = calculateTurbidityValue();
  // Serial.println(turbidityValue);

  // digitalWrite(2, phValue); // Adjust the threshold as needed
  // digitalWrite(3, tdsValue); // Adjust the threshold as needed
  // digitalWrite(4, turbidityValue); // Adjust the threshold as needed

  // Send data to NodeMCU through Serial
  Serial.print("D0:");
  Serial.print(phValue);
  Serial.print("D1:");
  Serial.print(tdsValue);
  Serial.print("D2:");
  Serial.print(turbidityValue);
  Serial.println();
  SUART.print("D0:");
  SUART.print(phValue);
  SUART.print("D1:");
  SUART.print(tdsValue);
  SUART.print("D2:");
  SUART.print(turbidityValue);
  SUART.println();

  delay(4000); // Delay for a short period before taking more readings
}

// Rest of your code remains unchanged
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