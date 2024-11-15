#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // For JSON parsing

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Wi-Fi credentials
const char* ssid = "ParasA33";
const char* password = "yashvi@1112";

// API URLs and keys
const char* weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=YourCity&appid=YourAPIKey";
const char* newsApiUrl = "https://newsapi.org/v2/top-headlines?country=us&apiKey=YourNewsAPIKey";

// Touch sensor pins
#define TOUCH_PIN1 32
#define TOUCH_PIN2 33
#define TOUCH_PIN3 25

int currentApp = 0; // 0: Date/Time, 1: Weather, 2: News

// Setup Wi-Fi connection
void setupWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

// Display Date and Time
void displayDateTime() {
  if (WiFi.status() == WL_CONNECTED) {
    configTime(0, 0, "pool.ntp.org");  // NTP server
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.printf("%04d-%02d-%02d %02d:%02d:%02d",
                     timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                     timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      display.display();
    }
  }
}

// Display Weather (using OpenWeatherMap API)
void displayWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(weatherApiUrl);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      float temp = doc["main"]["temp"];
      
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("Temperature: ");
      display.print(temp - 273.15);  // Kelvin to Celsius
      display.print(" C");
      display.display();
    }
    http.end();
  }
}

// Display News Feed (using News API)
void displayNews() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(newsApiUrl);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);
      String headline = doc["articles"][0]["title"];
      
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.print("Headline: ");
      display.print(headline);
      display.display();
    }
    http.end();
  }
}

// Setup touch sensors
void setupTouchSensors() {
  touchAttachInterrupt(TOUCH_PIN1, [](){currentApp = 0;}, 40);
  touchAttachInterrupt(TOUCH_PIN2, [](){currentApp = 1;}, 40);
  touchAttachInterrupt(TOUCH_PIN3, [](){currentApp = 2;}, 40);
}

void setup() {
  Serial.begin(115200);
  setupWiFi();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  setupTouchSensors();
}

void loop() {
  displayDateTime();
  // switch (currentApp) {
  //   case 0:
      
  //     break;
  //   case 1:
  //     displayWeather();
  //     break;
  //   case 2:
  //     displayNews();
  //     break;
  // }
  delay(2000);  // Update every 2 seconds
}
