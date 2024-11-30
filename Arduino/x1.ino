#define BLYNK_TEMPLATE_ID "YOUR_ID"
#define BLYNK_TEMPLATE_NAME "TTGO"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
#define CLOUDCHECKER V4

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ArduinoBLE.h>

// BLE characteristic handling
char auth[] = BLYNK_AUTHTOKEN;
char ssid[] = "Your_wifi_ssid";
char pass[] = "Your_wifi_password";
float distance = 0;  // Default value for distance
float battery = 0;   // Variable to store battery percentage

BLEService RadarService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEStringCharacteristic radarCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 55);

// Radar hardware pins and logic
#define RADAR_RX_PIN 20
#define RADAR_TX_PIN 21
#define VOLTAGE_PIN 2  // Use GPIO 2 to read voltage as per your instructions
HardwareSerial radarSerial(1);
int radarDistance;
String radarDistanceStr;

BLYNK_CONNECTED() {
  Blynk.setProperty(CLOUDCHECKER, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(CLOUDCHECKER, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(CLOUDCHECKER, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// Function to parse float from string (used in BLE processing)
float parseFloatFromString(String data, String prefix) {
  int startIndex = data.indexOf(prefix);
  if (startIndex == -1) return 0.0;
  startIndex += prefix.length();
  int endIndex = data.indexOf(',', startIndex);
  if (endIndex == -1) endIndex = data.length();
  String value = data.substring(startIndex, endIndex);
  return value.toFloat();
}

// Function to read radar data from the hardware
void readRadarData() {
  if (radarSerial.available()) {
    String radarData = "";
    char incomingByte;
    while (radarSerial.available()) {
      incomingByte = radarSerial.read();
      radarData += incomingByte;
    }
    Serial.println("Raw Radar Data: " + radarData);

    // Parse the radar data
    int startIndex = radarData.indexOf("ON");
    int rangeIndex = radarData.indexOf("Range", startIndex);
    if (startIndex != -1 && rangeIndex != -1) {
      String distanceStr = radarData.substring(rangeIndex + 6, rangeIndex + 9);
      radarDistance = distanceStr.toInt();
      radarDistanceStr = String(radarDistance);
      Serial.printf("Parsed Radar Distance: %d cm\n", radarDistance);
    } else {
      Serial.println("Failed to parse radar data.");
    }
  }
}

// Function to read battery voltage and calculate battery percentage
void readVoltage() {
  float read = analogRead(VOLTAGE_PIN);
  float voltage = (read / 4095.0) * 2 * 2.63 * 1.1;  // Calculate voltage
  battery = map(voltage, 3.2, 4.2, 0, 100);  // Map voltage to battery percentage (0-100%)
  Serial.printf("Battery Voltage: %.2f V, Battery Percentage: %.2f%%\n", voltage, battery);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Start BLE
  if (!BLE.begin()) {
    Serial.println("Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  // Start Blynk over Wi-Fi
  Blynk.begin(auth, ssid, pass);

  // BLE setup
  BLE.setLocalName("X2");
  BLE.setAdvertisedService(RadarService);
  RadarService.addCharacteristic(radarCharacteristic);
  BLE.addService(RadarService);
  BLE.advertise();
  Serial.println("BLE Radar Peripheral started");

  // Radar hardware initialization
  radarSerial.begin(115200, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);

  // Seed random generator for Wi-Fi random values
  randomSeed(analogRead(0));
}

void loop() {
  Blynk.run();  // Run Blynk to maintain connection over Wi-Fi

  // Read radar data (and send to V2 as string)
  readRadarData();
  Blynk.virtualWrite(V2, radarDistanceStr);  // Send radar data from hardware to V2 as a string

  // Read battery voltage and send percentage to V3
  readVoltage();
  Blynk.virtualWrite(V3, battery);  // Send battery percentage to V3

  // BLE handling: check if a BLE central device is connected
  BLEDevice central = BLE.central();
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // Don't block the loop, check for data without getting stuck
    if (radarCharacteristic.written()) {
      // If a value is received from BLE, process it
      Serial.print("Received BLE data: ");
      String receivedData = radarCharacteristic.value();
      Serial.println(receivedData);
      
      // Parse the received data for distance and send to V0
      distance = parseFloatFromString(receivedData, "Distance: ");
      Serial.println(distance);
      Blynk.virtualWrite(V0, distance);
    }
  }

  // Disconnect message
  if (!central) {
    Serial.println("Disconnected from central");
  }

  delay(2000);  // Wait 2 seconds before sending the next set of values
}
