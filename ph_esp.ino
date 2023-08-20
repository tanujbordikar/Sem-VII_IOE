#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const int NUM_SAMPLES = 10;
const int PH_SENSOR_PIN = A0;
const char* ssid = "B WLAN";
const char* password = "tanuj0067";
const char* server = "api.thingspeak.com";
String apiKey = "UWRAUM11GTO5Q5DK"; // Replace with your ThingSpeak channel's Write API Key

WiFiClient client;

void setup() {
  Serial.begin(9600);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  float pHValue = readpHValue(); // Read pH value from sensor
  
  // Prepare the data to be sent to ThingSpeak
  String data = "field1=" + String(pHValue);

  // Make an HTTP POST request to send the data to ThingSpeak
  if (client.connect(server, 80)) {
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");
    client.print(data);
    client.stop();
    Serial.println("Data sent to ThingSpeak");
  } else {
    Serial.println("Error connecting to ThingSpeak");
  }

  delay(15000); // Delay for 15 seconds before sending the next reading
}

float readpHValue() {
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
