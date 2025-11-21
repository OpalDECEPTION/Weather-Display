#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
const char* ssid = "EngineeringSubNet";
const char* password = "password";

// OpenWeatherMap API
const String api_key = "85c8420c016866562ddf5502749ac681";
const String city_name = "Medford,Ma,US";
const String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + city_name + "&appid=" + api_key + "&units=imperial";

// NeoPixel setup
#define PIXEL_PIN 10
#define NUM_PIXELS 25
Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Segment display setup
Adafruit_7segment display = Adafruit_7segment();


void setup() {
  Serial.begin(115200);

  // Start NeoPixels and display
  pixels.begin();
  pixels.setBrightness(128);
  display.begin(0x70);  // Default I2C address for HT16K33

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  fetchWeather();         //get and display the weather
  delay(15 * 60 * 1000);  // restart every 15 minutes
  ESP.restart();          //restart the ESP to avoid wifi outages and replay the code
  
}

void fetchWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(weatherUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        float temp_f = doc["main"]["temp"];
        String description = doc["weather"][0]["main"];
        String description2 = doc["weather"][0]["description"];

        // Display temperature
        int temp_int = round(temp_f);
        String temp_str = String(temp_int);

        // Add "F" at the end of display
        String display_str = temp_str + " F";

        // Send to display
        display.print(display_str);
        display.writeDisplay();
        pixels.clear();

        // Set pixel colors based on weather
        description.toLowerCase();
        if (description == "rain") {
          for (int i = 14; i < 19; i++) pixels.setPixelColor(i, pixels.Color(0, 102, 204));
        } else if (description == "snow") {
          for (int i = 20; i < 25; i++) pixels.setPixelColor(i, pixels.Color(128, 128, 128));
        } else if (description == "clear" || (description == "clouds" && (description2 == "few clouds" || description2 == "scattered clouds" || description2 == "broken clouds"))) {
          for (int i = 0; i < 4; i++) pixels.setPixelColor(i, pixels.Color(255, 219, 2));
        } else if (description == "clouds" || description == "smoke" || description == "haze" || description == "dust") {
          for (int i = 14; i < 18; i++) pixels.setPixelColor(i, pixels.Color(255, 255, 255));
        } else if (description == "drizzle" || description == "mist") {
          for (int i = 14; i < 19; i++) pixels.setPixelColor(i, pixels.Color(81, 117, 135));
        } else if (description == "thunderstorm") {
          for (int i = 8; i <= 10; i++) pixels.setPixelColor(i, pixels.Color(0, 102, 204));
          pixels.setPixelColor(6, pixels.Color(255, 255, 0));
        } else {
          Serial.println("Unknown description. Try again.");
        }

        pixels.show();
        Serial.printf("Temp: %.1f°F, Sky: %s\n", temp_f, description2.c_str());
      } else {
        Serial.println("JSON parse error");
      }
    } else {
      Serial.printf("HTTP GET failed: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi disconnected");
  }
}

void loop() {
}
