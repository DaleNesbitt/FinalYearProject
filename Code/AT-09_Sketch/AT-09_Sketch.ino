#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

// Time interval (in seconds) between each scan result printout
#define SCAN_INTERVAL 5  

// Pointer to BLE scan object
BLEScan* pBLEScan;

// Stores the last time results were printed
unsigned long lastPrint = 0;

void setup() {
  // Initialise serial communication for debugging output. 115200 seems to be the default for the AT-09
  Serial.begin(115200);              

  // Initialise the BLE device
  BLEDevice::init("");               

  // Create BLE scan object
  pBLEScan = BLEDevice::getScan();   

  // Use passive scanning (does not request additional data, lower power usage)
  pBLEScan->setActiveScan(false);    

  // Set scan interval. Number is in ms
  pBLEScan->setInterval(100);        

  // Set scan window (how long the radio listens during each interval)
  // Must be <= interval
  pBLEScan->setWindow(50);           
}

void loop() {
  // Get current system time (in milliseconds since boot)
  unsigned long now = millis();

  // Only perform scan and print results every SCAN_INTERVAL seconds
  if (now - lastPrint >= SCAN_INTERVAL * 1000) {
    lastPrint = now;

    Serial.println("=== Devices Nearby ===");

    // Start BLE scan for 2 seconds
    BLEScanResults* results = pBLEScan->start(2);

    // Get number of devices found during scan
    int count = results->getCount();

    // Loop through all detected BLE devices
    for (int i = 0; i < count; i++) {

      // Get individual device from scan results
      BLEAdvertisedDevice device = results->getDevice(i);

      // Extract MAC address as string
      String mac = device.getAddress().toString().c_str();

      // Extract RSSI (signal strength) and determine proximity to that device
      int rssi = device.getRSSI();

      // Filter out invalid or suspicious MAC addresses
      // Also ensures RSSI is a valid negative value (typical BLE RSSI range)
      if (rssi <= 0 && !isFakeMAC(mac)) {

        // Print device information to Serial Monitor
        Serial.print("Device: ");
        Serial.print(mac);
        Serial.print("  RSSI: ");
        Serial.println(rssi);
      }
    }

    Serial.println("----------------------");

    // Clear stored results to free memory before next scan
    pBLEScan->clearResults(); 
  }
}

// Function to detect obviously invalid or random MAC addresses
bool isFakeMAC(String mac) {

  // Convert MAC string to uppercase for consistent comparison
  mac.toUpperCase();

  // Reject MAC addresses that are all 0s or all Fs. These are obviously fake or randomised
  if (mac == "FF:FF:FF:FF:FF:FF" || mac == "00:00:00:00:00:00") return true;

  // Count number of 'F' and '0' characters in MAC
  // High counts may indicate randomised or invalid addresses
  int fCount = 0, zeroCount = 0;

  for (int i = 0; i < mac.length(); i++) {
    char c = mac[i];
    if (c == 'F') fCount++;
    if (c == '0') zeroCount++;
  }

  // If MAC is mostly F or 0, treat as invalid
  if (fCount >= 8 || zeroCount >= 8) return true;

  // Otherwise, consider MAC valid
  return false;
}