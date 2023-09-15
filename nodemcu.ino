#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h> 

// http://arduino.esp8266.com/stable/package_esp8266com_index.json

const char* ssid = "B WLAN"; // Replace with your WiFi SSID
const char* password = "tanuj0067"; // Replace with your WiFi password
const char* server = "api.thingspeak.com";
String apiKey = "UWRAUM11GTO5Q5DK"; // Replace with your ThingSpeak channel's Write API Key

WiFiClient client;
SoftwareSerial SUART(2, 3); // Replace rxPin and txPin with the actual pins you are using


void setup() {
  Serial.begin(115200);
  SUART.begin(115200); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  // Read the values from Arduino Uno through Serial
  // if (Serial.available() > 0) {
      // String data = Serial.readStringUntil('\n');
      // String data = Serial.write((char)SUART.read());
      Serial.write((char)SUART.read());
      String data="";
      Serial.println(data);
      
      // Extract values from the received data
      int phValue = getValue(data, "D0:");
      int tdsValue = getValue(data, "D1:");
      int turbidityValue = getValue(data, "D2:");
      
      // Print the values
      Serial.println("Received Data:");
      Serial.println("pH: " + String(phValue));
      Serial.println("TDS: " + String(tdsValue));
      Serial.println("Turbidity: " + String(turbidityValue));
      
      // Prepare the data to be sent to ThingSpeak
      String postData = "field1=" + String(phValue) + "&field2=" + String(tdsValue) + "&field3=" + String(turbidityValue);

      if (client.connect(server, 80)) {
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postData.length());
        client.print("\n\n");
        client.print(postData);
        client.stop();
        Serial.println("Data sent to ThingSpeak");
      // } else {
      //   Serial.println("Error connecting to ThingSpeak");
      // }
    delay(15000); // Delay for 15 seconds before sending the next reading
  }
}


int getValue(String data, String key) {
  int value = 0;
  int index = data.indexOf(key);
  if (index != -1) {
    value = data.substring(index + key.length()).toInt();
  }
  return value;
}