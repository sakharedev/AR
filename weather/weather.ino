#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi credentials
const char* ssid = "speed-Paras";
const char* password = "kyfaydaae";

// Latitude and Longitude for your location
const char* latitude = "18.485695";
const char* longitude = "74.017454";

// Open-Meteo API endpoint
String weatherApiUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + String(latitude) + "&longitude=" + String(longitude) + "&current_weather=true";

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }
  display.clearDisplay();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi!");
}

void displayWeather(float temperature) {
  // Clear the display
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Display the temperature
  display.print("Temp: ");
  display.print(temperature);
  display.print(" C");

  display.display();
}

void fetchWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(weatherApiUrl);
    int httpCode = http.GET();

    if (httpCode == 200) {  // HTTP_OK
      String payload = http.getString();
      Serial.println(payload);  // For debugging

      // Parse JSON
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      // Extract temperature
      float temperature = doc["current_weather"]["temperature"];

      // Display on OLED
      displayWeather(temperature);
    } else {
      Serial.print("Error fetching weather data. HTTP code: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

void loop() {
  // Fetch and display weather every 10 seconds
  fetchWeather();
  delay(10000);
}
