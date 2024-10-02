#define BLYNK_TEMPLATE_ID "TMPL3jYqDS4JQ"
#define BLYNK_TEMPLATE_NAME "Temperature"
#define BLYNK_AUTH_TOKEN "k-2MItSd9qdyh3pfpdd2oFYy1fqxSnvU"
#define CLOUDCHECKER V4

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ArduinoBLE.h>

#define RADAR_RX_PIN 20
#define RADAR_TX_PIN 21
#define VOLTAGE_PIN 2

HardwareSerial radarSerial(1);

float x1Voltage, x2Voltage, battery;
int radarDistance;
String radarDistanceStr, batteryStr;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Sathya _4G";
char pass[] = "palimar108";

BLEDevice central;
BLEService batteryService("12345678-1234-1234-1234-123456789abc");  // Same UUID as X2's service
BLEStringCharacteristic batteryCharacteristic("87654321-4321-4321-4321-abcdefabcdef", BLERead | BLEWrite, 10);  // Same UUID as X2's characteristic

// Radar configuration
#define LD2420_TOTAL_GATES 16
#define CMD_WRITE_REGISTER 0x0102

static const uint32_t FACTORY_MOVE_THRESH[LD2420_TOTAL_GATES] = {
    60000, 30000, 400, 250, 250, 250, 250, 250, 
    250, 250, 250, 250, 250, 200, 200, 200
};

static const uint32_t FACTORY_STILL_THRESH[LD2420_TOTAL_GATES] = {
    40000, 20000, 200, 200, 200, 200, 200, 150, 
    150, 100, 100, 100, 100, 100, 100, 100
};

// Function to send command to configure registers for gate thresholds
void setRegister(uint16_t reg, uint32_t value) {
  byte commandPacket[10];
  commandPacket[0] = 0xFA;
  commandPacket[1] = 0xFB;
  commandPacket[2] = 0xFC;
  commandPacket[3] = 0xFD;
  commandPacket[4] = 0x02;
  commandPacket[5] = 0x02;
  commandPacket[6] = (byte)(reg >> 8);
  commandPacket[7] = (byte)(reg & 0xFF);
  commandPacket[8] = (byte)(value >> 8);
  commandPacket[9] = (byte)(value & 0xFF);

  radarSerial.write(commandPacket, sizeof(commandPacket));
  Serial.printf("Set register 0x%04X to value 0x%X\n", reg, value);
}

// Function to configure radar gates
void configureGates() {
  Serial.println("Configuring gate thresholds...");

  for (uint8_t gate = 0; gate < LD2420_TOTAL_GATES; gate++) {
    setRegister(0x0010 + gate, FACTORY_MOVE_THRESH[gate]);
    delay(10);  
    setRegister(0x0020 + gate, FACTORY_STILL_THRESH[gate]);
    delay(10);
  }

  Serial.println("Gate thresholds configured successfully.");
}

// Function to read radar data
void readRadarData() {
  if (radarSerial.available()) {
    String radarData = "";
    char incomingByte;

    while (radarSerial.available()) {
      incomingByte = radarSerial.read();
      radarData += incomingByte;
    }

    Serial.println("Raw Radar Data: " + radarData);

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

// Function to read voltage and calculate battery percentage
void readVoltage() {
  float read = analogRead(VOLTAGE_PIN);
  x1Voltage = (read / 4095.0) * 2 * 2.63 * 1.1;
  battery = map(x1Voltage, 3.2, 4.2, 0, 100);
  batteryStr = String(battery);
}


void connectToX2() {
  central = BLE.central();  // Scan for a peripheral device

  if (central) {
    Serial.print("Connected to X2 module with address: ");
    Serial.println(central.address());

    // Discover services on the central device
    if (central.discoverService("12345678-1234-1234-1234-123456789abc")) {
      BLEService batteryService = central.service("12345678-1234-1234-1234-123456789abc");  // Get the service

      if (batteryService) {
        if (batteryService.hasCharacteristic("87654321-4321-4321-4321-abcdefabcdef")) {  // Check if the service has the characteristic
          BLECharacteristic batteryCharacteristic = batteryService.characteristic("87654321-4321-4321-4321-abcdefabcdef");

          if (batteryCharacteristic) {
            int valueLength = batteryCharacteristic.valueLength();
            uint8_t valueBuffer[valueLength];  // Create a buffer to hold the data

            batteryCharacteristic.readValue(valueBuffer, valueLength);  // Read the value into the buffer
            String x2Battery = String((char*)valueBuffer);  // Convert the buffer to a string

            Serial.printf("Received battery percentage from X2: %s%%\n", x2Battery.c_str());
            Blynk.virtualWrite(V4, x2Battery);  // Send received battery data to Blynk (V4)
          } else {
            Serial.println("Characteristic not found.");
          }
        } else {
          Serial.println("Service does not have the battery characteristic.");
        }
      } else {
        Serial.println("Battery service not found.");
      }
    } else {
      Serial.println("Failed to discover service on X2.");
    }
  }
}


void setup() {
  Serial.begin(115200);
  radarSerial.begin(115200, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);

  // Initialize Wi-Fi and Blynk connection
  Blynk.begin(auth, ssid, pass);
  Serial.println("Attempting to connect to Blynk...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to Wi-Fi...");
    delay(1000);
  }
  Serial.println("Connected to Wi-Fi.");

  configureGates();

  // Initialize BLE as central device
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed.");
    while (1);
  }
  Serial.println("BLE Central started...");
}

void loop() {
  Blynk.run();
  
  connectToX2();  // Connect to X2 and read battery data

  // Step 1: Read radar data
  readRadarData();
  Blynk.virtualWrite(V2, radarDistanceStr);

  // Step 2: Read X1 voltage and battery percentage
  readVoltage();
  Serial.printf("Battery Level: %s%%\n", batteryStr.c_str());
  Blynk.virtualWrite(V3, batteryStr);

  delay(500);
}
