#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  // Make sure to install ArduinoJson library in your Arduino IDE

// WiFi credentials
const char* ssid = "ParasA33";
const char* password = "yashvi@1112";

// Pushbullet API settings
const char* pushbulletToken = "o.67wKLAHGNYqAAdU0jOOqkMxbOzKq3a2M";  // Your Pushbullet Access Token
const char* pushbulletEndpoint = "https://api.pushbullet.com/v2/pushes";

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
}

// Function to get notifications from Pushbullet
void getPushbulletNotifications() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(pushbulletEndpoint);
    http.addHeader("Access-Token", pushbulletToken);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("Pushbullet Response:");
      Serial.println(payload);

      // Parse JSON response
      StaticJsonDocument<2000> doc;  // Adjust the size based on the response
      DeserializationError error = deserializeJson(doc, payload);
      
      if (!error) {
        JsonArray pushes = doc["pushes"].as<JsonArray>();
        
        for (JsonObject push : pushes) {
          const char* title = push["title"];
          const char* body = push["body"];

          // Print the notification details
          Serial.println("Notification:");
          Serial.print("Title: ");
          Serial.println(title);
          Serial.print("Body: ");
          Serial.println(body);

          // Optionally: Display notification on OLED or use it in other ways
        }
      } else {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.print("HTTP error: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
}

void loop() {
  getPushbulletNotifications();
  delay(1000);  // Check every 60 seconds
}
