#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Pin definitions for TTGO T-Display
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4

// Constructor for data object named tft 
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Wi-Fi and Blynk API details
const char* ssid = "Your_SSID";           // Replace with your Wi-Fi SSID
const char* password = "Your_PASSWORD";   // Replace with your Wi-Fi password
String blynkToken = "YOUR_AUTH_TOKEN";   // Replace with your Blynk token

String serverNameV0 = "https://blynk.cloud/external/api/get?token=" + blynkToken + "&V0";
String serverNameV2 = "https://blynk.cloud/external/api/get?token=" + blynkToken + "&V2";
String serverNameV3 = "https://blynk.cloud/external/api/get?token=" + blynkToken + "&V3";

float distance = 0, radarData = 0, randomValueV3 = 0;
bool wifiConnected = false;

// Previous values to track changes
float prevDistance = -1, prevRadarData = -1, prevRandomValueV3 = -1;

// Function to connect to Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(0, 10);
  tft.setTextSize(2);
  tft.print("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  wifiConnected = true;
  Serial.println("WiFi Connected!");
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 10);
  tft.print("WiFi Connected!");
  delay(2000);  // Give user time to see the "WiFi Connected!" message
}

// Function to fetch data from Blynk
void getData(String serverName, float& value) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);  
    int httpResponseCode = http.GET(); 

    if (httpResponseCode > 0) {
      String response = http.getString();
      value = response.toFloat();
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}

// Function to display values on TFT
void displayData() {
  // Set the text size and font properties once
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  
  // Clear only the specific text regions and update values
  if (distance != prevDistance) {
    tft.fillRect(0, 10, 240, 20, ST77XX_BLACK);  // Clear the region for V0 (Distance)
    tft.setCursor(0, 10);
    tft.setTextColor(ST77XX_GREEN);
    tft.printf("X2 (Battery): %.2f\n", distance); // Print value with 2 decimal places
    prevDistance = distance;
  }

  if (radarData != prevRadarData) {
    tft.fillRect(0, 50, 240, 20, ST77XX_BLACK);  // Clear the region for V2 (Radar Data)
    tft.setCursor(0, 50);
    tft.setTextColor(ST77XX_YELLOW);
    tft.printf("V2 (Radar): %.2f\n", radarData); // Print value with 2 decimal places
    prevRadarData = radarData;
  }

  if (randomValueV3 != prevRandomValueV3) {
    tft.fillRect(0, 90, 240, 20, ST77XX_BLACK);  // Clear the region for V3 (X1 Battery)
    tft.setCursor(0, 90);
    tft.setTextColor(ST77XX_CYAN);
    tft.printf("X1 (Battery): %.2f\n", randomValueV3); // Print value with 2 decimal places
    prevRandomValueV3 = randomValueV3;
  }
}

void setup(void) {
  Serial.begin(9600); 
  Serial.print(F("Hello! ST77xx TFT Test"));

  // Initialize TFT screen
  pinMode(TFT_BL, OUTPUT);      // TTGO T-Display enable Backlight pin 4
  digitalWrite(TFT_BL, HIGH);   // T-Display turn on Backlight
  tft.init(135, 240);           // Initialize ST7789 240x135
  Serial.println(F("Initialized"));

  // Connect to Wi-Fi
  connectToWiFi();
}

void loop() {
  if (wifiConnected) {
    // Fetch data from Blynk
    getData(serverNameV0, distance);
    getData(serverNameV2, radarData);
    getData(serverNameV3, randomValueV3);

    // Display fetched data on TFT only if changed
    displayData();

    delay(2000);  // Refresh every 2 seconds
  }
}
