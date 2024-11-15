#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi credentials
const char* ssid = "ParasA33";
const char* password = "yashvi@1112";

// News API endpoint and key
const char* newsApiKey = "398ec389871a40449548a7c77463892d";
String newsApiUrl = "https://newsapi.org/v2/top-headlines?country=us&apiKey=" + String(newsApiKey);

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Variables to hold headlines
String headlines[5];  // Array to store up to 5 headlines
int headlineCount = 0;
int currentHeadlineIndex = 0;

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

  // Initial fetch of news headlines
  fetchNews();
}

// void displayHeadline(String headline) {
//   display.clearDisplay();
//   display.setTextSize(1);
//   display.setTextColor(SSD1306_WHITE);
//   display.setCursor(0, 0);

//   // Display the headline
//   display.print("Headline: ");
//   display.println(headline);
//   display.display();
// }
void displayHeadline(String headline) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  int lineHeight = 8;            // Height of each line
  int maxCharsPerLine = SCREEN_WIDTH / 6;  // Approximate max chars per line at text size 1

  display.print("Headline: ");
  int y = lineHeight;

  String line = "";
  String word = "";

  for (int i = 0; i < headline.length(); i++) {
    char c = headline[i];

    // Build up the word until a space or the end of the headline
    if (c == ' ' || i == headline.length() - 1) {
      if (i == headline.length() - 1 && c != ' ') {
        word += c;  // Add the last character if it's not a space
      }

      // Check if adding the word with a space would exceed max line length
      if (line.length() + word.length() + 1 <= maxCharsPerLine) {
        if (line.length() > 0) {
          line += " ";  // Add a space before the word if the line isn't empty
        }
        line += word;  // Add the word to the line
      } else {
        // Print the current line and start a new line with the word
        display.setCursor(0, y);
        display.println(line);
        y += lineHeight;

        // Stop if reaching the bottom of the display
        if (y >= SCREEN_HEIGHT - lineHeight) {
          break;
        }

        line = word;  // Start new line with the word
      }

      word = "";  // Clear the word
    } else {
      word += c;  // Accumulate characters into the current word
    }
  }

  // Print any remaining text in the line
  if (line.length() > 0) {
    display.setCursor(0, y);
    display.println(line);
  }

  display.display();
}

void fetchNews() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(newsApiUrl);
    int httpCode = http.GET();

    if (httpCode == 200) {  // HTTP_OK
      String payload = http.getString();
      Serial.println(payload);  // For debugging

      // Parse JSON
      DynamicJsonDocument doc(4096);
      deserializeJson(doc, payload);

      // Clear the headlines array
      headlineCount = 0;

      // Extract the first 5 article headlines
      for (int i = 0; i < 5; i++) {
        String headline = doc["articles"][i]["title"].as<String>();
        if (headline.length() > 0) {
          headlines[headlineCount] = headline;
          headlineCount++;
        }
      }
      
      // Reset the index to start cycling from the beginning
      currentHeadlineIndex = 0;
    } else {
      Serial.print("Error fetching news. HTTP code: ");
      Serial.println(httpCode);
    }
    http.end();
  }
}

void loop() {
  // Display the current headline
  if (headlineCount > 0) {
    displayHeadline(headlines[currentHeadlineIndex]);
    currentHeadlineIndex = (currentHeadlineIndex + 1) % headlineCount;  // Move to next headline
  }

  // Delay for 30 seconds
  delay(5000);

  // Refresh headlines after cycling through all of them
  if (currentHeadlineIndex == 0) {
    fetchNews();
  }
}