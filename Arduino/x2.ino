#include <ArduinoBLE.h>

float voltage = 0;
int battery = 0;

String radarData = "";  // The string you want to send
String distanceStr = "13";

// Function to read battery voltage and calculate battery percentage
void readVoltage() {
  float read = analogRead(A0);
  voltage = (read / 4095) * 2 * 2.63 * 1.1;
  battery = map(voltage, 3.2, 4.2, 0, 100);  // Map the voltage to a percentage (0-100)
}

// Function to measure radar data along with voltage and battery
void radarDataMeasure() {
  // Generate a random distance between 10 and 100 cm
  int randomDistance = random(10, 100);
  
  // Convert the random distance to a string
  distanceStr = String(randomDistance);

  // Read the battery voltage and percentage
  readVoltage();

  // Construct the radar data string with the voltage, battery percentage, and random distance
  radarData = "voltage: " + String(voltage, 2) + ", Battery: " + String(battery) + "%, Charging: 1, Distance: " + distanceStr + " cm,";  
}

void controlRadar(BLEDevice peripheral){
  if (peripheral.connect()) {
    // Connection established
  } else {
    return;
  }

  if (peripheral.discoverAttributes()) {
    // Attributes discovered
  } else {
    peripheral.disconnect();
    return;
  }

  BLECharacteristic RadarCharacteristic = peripheral.characteristic("19b10001-e8f2-537e-4f6c-d104768A1214");

  if (!RadarCharacteristic || !RadarCharacteristic.canWrite()) {
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    radarDataMeasure();
    
    // Convert String to byte array
    int strLen = radarData.length() + 1; // +1 for null terminator
    char data[strLen];
    radarData.toCharArray(data, strLen);
    
    // Write the byte array to the characteristic
    RadarCharacteristic.writeValue((uint8_t*)data, strLen);
    
    delay(1000);  // Send data every second
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  BLE.begin();
  BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768A1214");
}

void loop() {
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    if (peripheral.localName() == "X2") {
      controlRadar(peripheral);
    }
    BLE.scanForUuid("19b10000-e8f2-537e-4f6c-d104768A1214");
  }
}
