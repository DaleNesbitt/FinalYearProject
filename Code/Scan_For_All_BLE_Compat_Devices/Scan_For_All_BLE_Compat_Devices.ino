// Include BLE libraries for ESP32
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Pointer to a BLEScan object
BLEScan* pBLEScan;

void setup() {
  // Start serial monitor for debugging
  Serial.begin(115200);

  // Initialize BLE
  BLEDevice::init("");

  // Get a pointer to the BLE scanner
  pBLEScan = BLEDevice::getScan();

  // Enable active scanning
  // This means the ESP32 will request extra data from detected devices
  pBLEScan->setActiveScan(true);
}

void loop() {
  Serial.println("Scanning for BLE devices...");
  
  // Start scanning for 5 seconds
  // 'false' means we don't continue scanning in background after this
  // Store results as a pointer
  BLEScanResults* pFoundDevices = pBLEScan->start(5, false);

  // Loop through all detected BLE devices
  for (int i = 0; i < pFoundDevices->getCount(); i++) {

    // Get each detected device
    BLEAdvertisedDevice device = pFoundDevices->getDevice(i);

    // Print MAC address
    Serial.print("MAC: ");
    Serial.print(device.getAddress().toString().c_str());

    // Print device name if it has one
    Serial.print(" | Name: ");
    Serial.print(device.getName().c_str());

    // Print RSSI value
    Serial.print(" | RSSI: ");
    Serial.println(device.getRSSI());
  }

  Serial.println("------ SCAN END ------\n");

  // Clear results to free memory
  pBLEScan->clearResults();

  // Wait 2 seconds before scanning again
  delay(2000);
}
