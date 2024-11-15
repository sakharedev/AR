#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi credentials
const char* ssid = "ParasA33";
const char* password = "yashvi@1112";

// Weather API details
const char* latitude = "18.485695";
const char* longitude = "74.017454";
String weatherApiUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + String(latitude) + "&longitude=" + String(longitude) + "&current_weather=true";

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// NTP client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);  // Update every 60 seconds

// Set time offset for local timezone (e.g., UTC+5:30 = 19800 seconds)
const long utcOffsetInSeconds = 5 * 3600 + 30 * 60;

// Variables for weather data
float temperature = 0;
bool showWeather = true;  // Toggle between weather and date/time

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

  // Initialize NTP client
  timeClient.setTimeOffset(utcOffsetInSeconds);
  timeClient.begin();
  fetchWeather();  // Initial fetch of weather data
}

void displayDateTime() {
  // Update NTP time
  timeClient.update();
  
  // Get the current epoch time adjusted for time zone
  unsigned long localEpochTime = timeClient.getEpochTime();

  // Convert epoch time to human-readable date and time
  int currentSecond = localEpochTime % 60;
  int currentMinute = (localEpochTime / 60) % 60;
  int currentHour = (localEpochTime / 3600) % 24;
  
  // Calculate the number of days since the epoch and correct for leap years
  unsigned long days = localEpochTime / 86400L;
  int currentYear = 1970;
  
  while (days >= (isLeapYear(currentYear) ? 366 : 365)) {
    days -= isLeapYear(currentYear) ? 366 : 365;
    currentYear++;
  }

  // Determine the month and day
  int monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (isLeapYear(currentYear)) monthDays[1] = 29;

  int currentMonth = 0;
  while (days >= monthDays[currentMonth]) {
    days -= monthDays[currentMonth];
    currentMonth++;
  }
  int currentDay = days + 1;

  // Display date and time
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Display date
  display.printf("Date: \n%02d-%02d-%04d\n", currentDay, currentMonth + 1, currentYear);

  // Display time
  display.printf("Time: \n%02d:%02d:%02d", currentHour, currentMinute, currentSecond);
  display.display();
}

// Function to check if a year is a leap year
bool isLeapYear(int year) {
  return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

void displayWeather() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Temp: \n");
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
      temperature = doc["current_weather"]["temperature"].as<float>();

      Serial.println("Temperature: " + String(temperature));
    } else {
      Serial.print("Error fetching weather data. HTTP code: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

void loop() {
  // Toggle between displaying weather and date/time
  if (showWeather) {
    displayWeather();
  } else {
    displayDateTime();
  }

  // Wait for 10 seconds before updating
  delay(10000);

  // Toggle view and refresh weather data if going back to weather display
  showWeather = !showWeather;
  if (showWeather) {
    fetchWeather();
  }
}