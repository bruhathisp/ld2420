
/*
 * Code to configure gates and extract distance from LD2420 radar module.
 * 
 * Circuit Connections:
 *   Radar Module      ESP32
 *       3V3       -    3V3
 *       GND       -    GND
 *       OT1       -    GPIO4 (can be changed)
 *       RX        -    GPIO16 (can be changed)
 */

#define RADAR_RX_PIN 16
#define RADAR_TX_PIN 17

#define LD2420_TOTAL_GATES 16
#define CMD_WRITE_REGISTER 0x0102

// Updated Gate configuration thresholds (move and still thresholds)
static const uint32_t FACTORY_MOVE_THRESH[LD2420_TOTAL_GATES] = {
    60000, 30000, 400, 250, 250, 250, 250, 250, 
    250, 250, 250, 250, 250, 200, 200, 200
};

static const uint32_t FACTORY_STILL_THRESH[LD2420_TOTAL_GATES] = {
    40000, 20000, 200, 200, 200, 200, 200, 150, 
    150, 100, 100, 100, 100, 100, 100, 100
};

HardwareSerial radarSerial(1);

// Function to send a command to configure registers for gate thresholds
void setRegister(uint16_t reg, uint32_t value) {
  byte commandPacket[10];
  commandPacket[0] = 0xFA;  // Command header
  commandPacket[1] = 0xFB;
  commandPacket[2] = 0xFC;
  commandPacket[3] = 0xFD;
  commandPacket[4] = 0x02;  // Data length high byte
  commandPacket[5] = 0x02;  // Data length low byte
  commandPacket[6] = (byte)(reg >> 8);
  commandPacket[7] = (byte)(reg & 0xFF);
  commandPacket[8] = (byte)(value >> 8);
  commandPacket[9] = (byte)(value & 0xFF);

  radarSerial.write(commandPacket, sizeof(commandPacket));
  Serial.printf("Set register 0x%04X to value 0x%X\n", reg, value);
}

// Function to configure gates
void configureGates() {
  Serial.println("Configuring gate thresholds...");

  for (uint8_t gate = 0; gate < LD2420_TOTAL_GATES; gate++) {
    // Set the move threshold for each gate
    setRegister(0x0010 + gate, FACTORY_MOVE_THRESH[gate]);
    delay(10);  // Delay to prevent command collision
    // Set the still threshold for each gate
    setRegister(0x0020 + gate, FACTORY_STILL_THRESH[gate]);
    delay(10);
  }

  Serial.println("Gate thresholds configured successfully.");
}

void setup() {
  // Initialize Serial Monitor and Radar Serial
  Serial.begin(115200);
  radarSerial.begin(115200, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN);
  Serial.println("ESP32 is ready to configure LD2420 radar sensor and receive distance data...");

  // Configure gate thresholds
  configureGates();
}

void loop() {
  if (radarSerial.available()) {
    String radarData = "";
    char incomingByte;

    // Read radar data from Serial1
    while (radarSerial.available()) {
      incomingByte = radarSerial.read();
      radarData += incomingByte;
    }

    // Process the data if it contains the "ON" header and "Range"
    int startIndex = radarData.indexOf("ON");
    int rangeIndex = radarData.indexOf("Range", startIndex);

    if (startIndex != -1 && rangeIndex != -1) {
      // The distance data starts 6 characters after the "Range" word
      String distanceStr = radarData.substring(rangeIndex + 6, rangeIndex + 9);
      int distance = distanceStr.toInt();  // Convert distance string to integer

      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    }
  }

  delay(500);  // Adjust as needed
}
