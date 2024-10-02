#include <ArduinoBLE.h>

BLEService batteryService("12345678-1234-1234-1234-123456789abc");  // Create a BLE Service
BLEStringCharacteristic batteryCharacteristic("87654321-4321-4321-4321-abcdefabcdef", BLERead | BLEWrite, 10);  // Create a BLE Characteristic to store the battery percentage

float batteryPercentage = 75;  // Example battery percentage for X2

void setup() {
  Serial.begin(115200);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed.");
    while (1);  // If BLE fails to start, enter an infinite loop
  }

  // Set BLE Local Name and service
  BLE.setLocalName("X2_Module");  // Name of the X2 device
  BLE.setAdvertisedService(batteryService);  // Advertise the battery service
  batteryService.addCharacteristic(batteryCharacteristic);  // Add the battery characteristic to the service
  BLE.addService(batteryService);  // Add the service
  BLE.advertise();  // Start advertising
  Serial.println("BLE service started, advertising as X2_Module...");

  // Send initial battery percentage
  batteryCharacteristic.writeValue(String(batteryPercentage));  // Write initial battery percentage
}

void loop() {
  BLEDevice central = BLE.central();  // Wait for a central device to connect

  // If a central device is connected
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());  // Print the central device's address

    // Send updated battery percentage every 2 seconds
    while (central.connected()) {
      batteryPercentage = random(20, 101);  // Simulate random battery percentage between 20% and 100%
      batteryCharacteristic.writeValue(String(batteryPercentage));  // Write the updated battery percentage to the characteristic
      Serial.printf("Sent battery percentage to X1: %.2f%%\n", batteryPercentage);

      delay(2000);  // Wait 2 seconds before sending the next update
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }

  delay(1000);  // Wait before the next BLE check
}
