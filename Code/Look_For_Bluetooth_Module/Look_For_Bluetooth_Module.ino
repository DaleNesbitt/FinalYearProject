#include <BLEDevice.h>
#include <BLEScan.h>

// Pointer
BLEScan* pBLEScan;

void setup() {
  // Initialise the Serial Monitor for debugging output
  Serial.begin(115200);

  // Initialise BLE stack on ESP32
  BLEDevice::init("");

  // Create BLE scan object
  pBLEScan = BLEDevice::getScan();

  // Enable active scanning
  pBLEScan->setActiveScan(true);
}

void loop() {

  // header
  Serial.println("=== Devices Nearby ===");

  // Start BLE scan for 3 seconds
  // Second parameter 'false' means "do not keep previous results", so, it's a fresh scan every time
  BLEScanResults* results = pBLEScan->start(3, false);

  // Loop through all detected BLE devices
  for (int i = 0; i < results->getCount(); i++) {

    // Get device from scan results
    BLEAdvertisedDevice device = results->getDevice(i);

    // Print MAC address of detected device
    Serial.print("Device: ");
    Serial.print(device.getAddress().toString().c_str());

    // Print RSSI (signal strength in dBm)
    Serial.print("  RSSI: ");
    Serial.println(device.getRSSI());
  }

  // Delay before next scan cycle to avoid non-stop scanning
  delay(8000);
}