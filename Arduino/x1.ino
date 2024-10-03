#define BLYNK_TEMPLATE_ID "TMPL3jKCoYK-i"
#define BLYNK_TEMPLATE_NAME "Test"
#define BLYNK_AUTHTOKEN "k-2MItSd9qdyh3pfpdd2oFYy1fqxSnvU"
#define CLOUDCHECKER V4

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ArduinoBLE.h>

char auth[] = BLYNK_AUTHTOKEN;
char ssid[] = "Sathya _4G";
char pass[] = "palimar108";
float distance = 0;  // Default value for distance

BLEService RadarService("19B10000-E8F2-537E-4F6C-D104768A1214");
BLEStringCharacteristic radarCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 55);

BLYNK_CONNECTED() {
  Blynk.setProperty(CLOUDCHECKER, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(CLOUDCHECKER, "onImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(CLOUDCHECKER, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

float parseFloatFromString(String data, String prefix) {
  int startIndex = data.indexOf(prefix);
  if (startIndex == -1) return 0.0;
  startIndex += prefix.length();
  int endIndex = data.indexOf(',', startIndex);
  if (endIndex == -1) endIndex = data.length();
  String value = data.substring(startIndex, endIndex);
  return value.toFloat();
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // Start BLE
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
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

  // Seed random generator for Wi-Fi random values
  randomSeed(analogRead(0));
}

void loop() {
  Blynk.run();  // Run Blynk to maintain connection over Wi-Fi

  // Update random values for V2 and V3 continuously
  float randomValueV2 = random(100, 1000) / 10.0;  // Random float between 10.0 and 100.0
  float randomValueV3 = random(100, 500) / 10.0;   // Random float between 10.0 and 50.0

  // Send random values to V2 and V3
  Blynk.virtualWrite(V2, randomValueV2);
  Blynk.virtualWrite(V3, randomValueV3);

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
      
      // Parse the received data for distance
      distance = parseFloatFromString(receivedData, "Distance: ");
      Serial.println(distance);

      // Send the distance value to V0
      Blynk.virtualWrite(V0, distance);
    }
  }

  // Disconnect message
  if (!central) {
    Serial.println("Disconnected from central");
  }

  delay(2000);  // Wait 2 seconds before sending the next set of values
}




