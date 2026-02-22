#include "arduino_secrets.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "your wifi";
const char* password = "your wifi password";

const char* host = "api.groq.com";
const int httpsPort = 443;

String apiKey = "valid groq API key";

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);

  // Link-port pins
  pinMode(2, INPUT_PULLUP);  // D0
  pinMode(3, INPUT_PULLUP);  // D1

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  client.setInsecure(); 
}

String sendGroq(String userMessage) {
  if (!client.connect(host, httpsPort)) {
    return "Connection failed";
  }

  String payload = 
    "{\"model\":\"llama-3.1-8b-instant\","
    "\"messages\":[{\"role\":\"user\",\"content\":\"" + userMessage + "\"}]}";

  String request =
    String("POST /openai/v1/chat/completions HTTP/1.1\r\n") +
    "Host: " + host + "\r\n" +
    "Authorization: Bearer " + apiKey + "\r\n" +
    "Content-Type: application/json\r\n" +
    "Content-Length: " + payload.length() + "\r\n\r\n" +
    payload;

  client.print(request);

  String response = "";
  long timeout = millis() + 5000;

  while (millis() < timeout) {
    while (client.available()) {
      response += client.readString();
    }
  }

  int contentIndex = response.indexOf("\"content\":\"");
  if (contentIndex == -1) return response;

  contentIndex += 11;
  int endIndex = response.indexOf("\"", contentIndex);
  return response.substring(contentIndex, endIndex);
}

void loop() {
  String reply = sendGroq("hey groq, what's 1+1?");
  Serial.println("Groq:");
  Serial.println(reply);

  delay(5000);
}