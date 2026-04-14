#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Pointer to BLE scanning object
BLEScan* pBLEScan;

// Hardcoded MAC address of target BLE device (AT-09 module)
String targetMAC = "33:b1:d4:75:62:d4";


// Custom callback class triggered whenever a BLE device is discovered
class DeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

  // This function runs automatically for every detected BLE device
  void onResult(BLEAdvertisedDevice device) {

    // Extract MAC address of detected device
    String address = device.getAddress().toString().c_str();

    // Check if detected device matches the target MAC
    if(address == targetMAC) {

      // Read RSSI (signal strength in dBm)
      int rssi = device.getRSSI();

      // Print detection message
      Serial.print("AT-09 detected | RSSI: ");
      Serial.println(rssi);

      // -------- PROXIMITY CLASSIFICATION --------
      // Classify device distance based on RSSI thresholds

      if(rssi > -65) {
        Serial.println("STATE: BAG VERY NEAR");
      }
      else if(rssi > -75) {
        Serial.println("STATE: BAG NEAR");
      }
      else if(rssi > -90) {
        Serial.println("STATE: BAG FAR");
      }
      else {
        Serial.println("STATE: BAG POSSIBLY LOST");
      }

      Serial.println();
    }
  }
};


void setup() {

  // Initialise Serial Monitor for debugging output
  Serial.begin(115200);

  // Initialise BLE stack
  BLEDevice::init("");

  // Create BLE scan object
  pBLEScan = BLEDevice::getScan();

  // Attach callback so each discovered device triggers onResult()
  pBLEScan->setAdvertisedDeviceCallbacks(new DeviceCallbacks());

  // Enable active scanning
  pBLEScan->setActiveScan(true);
}


void loop() {

  // Start BLE scan for 3 seconds
  // 'false' = do not accumulate old results
  pBLEScan->start(3, false);

  // Delay between scans to control scan frequency
  delay(2000);
}