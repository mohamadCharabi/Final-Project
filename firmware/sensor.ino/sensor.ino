#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

const char* WIFI_SSID     = "Major Electric";
const char* WIFI_PASSWORD = "2672463555";
const char* API_URL       = "http://httpbin.org/post";

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectToWiFi();
}

void loop() {
  delay(5000);

  float humidity    = dht.readHumidity();
  float temperature = dht.readTemperature(true);

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read sensor!");
    return;
  }

  Serial.printf("Humidity: %.1f%%  Temp: %.1fF\n", humidity, temperature);
  sendReading(temperature, humidity);
}

void connectToWiFi() {
  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
}

void sendReading(float temperature, float humidity) {
  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<128> doc;
  doc["temperature"] = temperature;
  doc["humidity"]    = humidity;

  String payload;
  serializeJson(doc, payload);

  int responseCode = http.POST(payload);
  Serial.printf("POST response: %d\n", responseCode);
  http.end();
}