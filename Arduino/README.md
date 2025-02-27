## blynk


``` C
#define BLYNK_TEMPLATE_ID "TMPL3jKCoYK-i"
#define BLYNK_TEMPLATE_NAME "Test"
#define BLYNK_AUTHTOKEN ""
#define CLOUDCHECKER V4

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ArduinoBLE.h>

char auth[] = BLYNK_AUTHTOKEN;
char ssid[] = "";
char pass[] = "";
float distance;

BLEService RadarService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLEStringCharacteristic radarCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite, 55);

BLYNK_CONNECTED()
{
  Blynk.setProperty(CLOUDCHECKER, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(CLOUDCHECKER, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
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

void setup(){
  Serial.begin(9600);
  while(!Serial);

  if(!BLE.begin()){
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  Blynk.begin(auth, ssid, pass);

  BLE.setLocalName("X2");
  BLE.setAdvertisedService(RadarService);
  RadarService.addCharacteristic(radarCharacteristic);
  BLE.addService(RadarService);
  // radarCharacteristic.writeValue(0);
  BLE.advertise();
  Serial.println("BLE Radar Peripheral");
}

void loop(){
  BLEDevice central = BLE.central();

  if(central){
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    while(central.connected()){
      Blynk.run();
      // Blynk.virtualWrite()
      if(radarCharacteristic.written()){
        Serial.print("Received value: ");
        String receivedData = radarCharacteristic.value();
        Serial.println(receivedData);
        distance = parseFloatFromString(receivedData,"Distance: ");
        Serial.println(distance);
        Blynk.virtualWrite(V0,distance);
      }
    }
    Serial.println("Disconnected from central");
  }
}

```


## periph   

``` C
#include <ArduinoBLE.h>

String radarData = "";  // The string you want to send
String distanceStr = "13";

void radarDataMeasure() {
  // Generate a random distance between 10 and 100 cm
  int randomDistance = random(10, 100);
  
  // Convert the random distance to a string
  distanceStr = String(randomDistance);

  // Construct the radar data string with the random distance
  radarData = "voltage: 3.52, Battery: 32, Charging: 1, Distance: " + distanceStr + " cm,";  
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

```
