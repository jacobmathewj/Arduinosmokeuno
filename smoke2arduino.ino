#include <SoftwareSerial.h>

SoftwareSerial esp8266(2, 3); // RX, TX
const char* ssid = "lazer.net";
const char* password = "wifi password of yours";
const char* server = "api.thingspeak.com";
const char* apiKey = "ZE86YS51PSVK8C65"; // ThingSpeak API Key//pls choose ewpective key of your  channel

const int gasSensorPin = A0;//you can choose any analog pins
const int interval = 2000; // Send data every 2 seconds//change this depends upon your purpose

// Manually set latitude and longitude
const float latitude = 9.6779; // Example latitude
const float longitude = 76.5430; // Example longitude

void setup() {
  Serial.begin(115200);
  while (!Serial);
  esp8266.begin(115200);
  delay(1000);
  connectToWiFi();
}

void loop() {
  int gasValue = analogRead(gasSensorPin); // Read the raw analog value
  Serial.print("Gas sensor value: ");
  Serial.println(gasValue);
  sendToThingSpeak(gasValue);
  delay(interval);
}

void connectToWiFi() {//AT commands for wifi module
  sendCommand("AT+RST", 1000);
  sendCommand("AT+CWMODE=1", 1000);
  sendCommand("AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"", 4000);
}

void sendToThingSpeak(int value) {
  if (sendCommand("AT+CIPSTART=\"TCP\",\"" + String(server) + "\",80", 2000)) {
    String postStr = "field1=" + String(value) + "&field2=" + String(latitude) + "&field3=" + String(longitude);
    String postRequest = "POST /update HTTP/1.1\nHost: " + String(server) + "\nConnection: close\nX-THINGSPEAKAPIKEY: " + String(apiKey) + "\nContent-Type: application/x-www-form-urlencoded\nContent-Length: " + String(postStr.length()) + "\n\n" + postStr;
    String sendCmd = "AT+CIPSEND=" + String(postRequest.length());
    if (sendCommand(sendCmd, 500)) {
      if (sendCommand(postRequest, 1000)) {
        sendCommand("AT+CIPCLOSE", 100);
      }
    }
  }
}

boolean sendCommand(String command, int timeout) {
  esp8266.println(command);
  long startTime = millis();
  while ((millis() - startTime) < timeout) {
    if (esp8266.find("OK")) {
      return true;
    }
  }
  return false;
}
