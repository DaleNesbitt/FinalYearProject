#include <Arduino.h>

// ---------------- CONFIGURATION ----------------

// Time interval (seconds) between BLE scan attempts when searching for device
const int scanIntervalSec = 3;

// Time interval (seconds) for RSSI monitoring once device is locked
const float rssiIntervalSec = 2.5;

// RSSI thresholds used to classify proximity (lower = stronger signal)
const int rssiVeryNearMax = 55;     // RSSI < 55  = very near
const int rssiNearMax = 65;         // RSSI < 65  = near
const int rssiFarMax = 85;          // RSSI < 85  = far, otherwise lost


// ---------------- GEOFENCE DEFINITION ----------------

// Structure representing a rectangular geofence using latitude/longitude bounds
// This will be used to define if the device has lef tthe area which will trigger the call to the GPS
struct GeoFence {
  float minLat, maxLat, minLon, maxLon;
} geofence = {51.500, 51.505, -0.125, -0.120}; // Example bounded region


// ---------------- SIMULATED DEVICES ----------------

// Structure representing a BLE device (simulated)
struct Device {
  String name;
  int rssi;       // Simulated RSSI value
  bool present;   // Whether device is currently "visible"
};

// Array of simulated devices (used instead of unreliable hardware)
Device devices[] = {
  {"AT-09", -50, true},
};


// ---------------- GLOBAL STATE ----------------

// Stores user-defined target device name
String targetDevice = "";

// Indicates whether system is currently locked onto a detected device
bool lockedOn = false;


// ---------------- FUNCTION DECLARATIONS ----------------

String normalizeString(String s);
void requestGPSCheck();
bool checkGeofence(float lat, float lon);
String getProximity(int rssi);
String readLineFromSerial();
int randomRSSI();
void sendAlert();


// ---------------- SETUP ----------------

void setup() {
  // Initialise Serial Monitor for interaction and debugging
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("=== BLE & GPS Simulation Started ===");
  Serial.println("Enter target device name:");
}


// ---------------- MAIN LOOP ----------------

void loop() {

  // -------- USER INPUT PHASE --------
  // Allows user to input a target BLE device name. We want the BLE module (AT-09)
  if (!lockedOn && Serial.available()) {
    String inputName = normalizeString(readLineFromSerial());

    if (inputName.length() > 0) {
      targetDevice = inputName;
      lockedOn = false;

      Serial.println("Target device updated to: " + targetDevice);
    }
  }

  // -------- BLE SCANNING PHASE --------
  // Attempts to locate target device in simulated environment
  if (!lockedOn && targetDevice.length() > 0) {

    Serial.println("\nScanning for device: " + targetDevice + " ...");

    bool found = false;

    // Iterate through simulated devices (single device in this case)
    for (int i = 0; i < 1; i++) {

      // Compare normalized device names and check presence
      if (normalizeString(devices[i].name) == targetDevice && devices[i].present) {
        found = true;
        lockedOn = true;

        Serial.println("Device found! Locking onto " + devices[i].name);
        break;
      }
    }

    if (!found) {
      Serial.println("Device not found, scanning again...");
    }

    // Wait before next scan attempt
    delay(scanIntervalSec * 1000);
  }

  // -------- RSSI MONITORING PHASE --------
  else if (lockedOn) {

    int rssiValue = 0;

    Serial.println("\nEnter current RSSI:");

    unsigned long startMillis = millis();
    bool inputReceived = false;

    // Wait for user input within defined interval
    while ((millis() - startMillis) < (unsigned long)(rssiIntervalSec * 1000)) {

      if (Serial.available()) {
        String line = readLineFromSerial();

        if (line.length() > 0) {
          rssiValue = line.toInt();
          inputReceived = true;
        }
        break;
      }
    }

    // If no user input, simulate RSSI value
    if (!inputReceived) {
      rssiValue = randomRSSI();
      Serial.println("Current RSSI: " + String(rssiValue));
    }

    // Determine proximity classification from RSSI
    String proximity = getProximity(rssiValue);

    Serial.println("RSSI: " + String(rssiValue) + " | Proximity: " + proximity);

    // -------- HANDSHAKE TRIGGER --------
    // If device is considered lost, trigger GPS validation
    if (proximity == "lost") {

      Serial.println("Proximity indicates device lost. Requesting GPS confirmation...");

      requestGPSCheck();
    }

    delay(rssiIntervalSec * 1000);
  }
}


// ---------------- FUNCTIONS ----------------

// Normalises string input by removing spaces and converting to lowercase
String normalizeString(String s) {
  s.replace(" ", "");
  s.toLowerCase();
  return s;
}


// Converts RSSI value into proximity category
String getProximity(int rssi) {
  if (rssi < rssiVeryNearMax) return "very near";
  if (rssi < rssiNearMax) return "near";
  if (rssi < rssiFarMax) return "far";
  return "lost";
}


// Requests GPS input from user and evaluates geofence condition
void requestGPSCheck() {

  // If the BLE device was out of range it would call the GPS module for an absolute location
  Serial.println("\nRSSI indicates device lost.");
  // The GPS would come back with the absolute location of the device and define if it's still within the geofenced location
  Serial.println("Enter GPS coordinates as lat,lon (Range is: 51.500, 51.505, -0.125, -0.120):  ");

  float lat = 0.0, lon = 0.0;
  bool valid = false;

  // Wait until valid input format is received
  while (!valid) {

    String line = normalizeString(readLineFromSerial());

    int commaIndex = line.indexOf(',');

    if (commaIndex > 0 && commaIndex < line.length() - 1) {

      lat = line.substring(0, commaIndex).toFloat();
      lon = line.substring(commaIndex + 1).toFloat();

      valid = true;

    } else {
      Serial.println("Invalid input. Format: lat,lon");
    }
  }

  // Evaluate whether coordinates fall inside geofence
  if (checkGeofence(lat, lon)) {

    Serial.println("Device has NOT left geofenced location. BLE loss was false positive.");
    Serial.println("Continuing to monitor " + devices[0].name);

    lockedOn = true;

  } else {

    Serial.println("Device has LEFT the area without the BLE device!");

    // Trigger alert event
    sendAlert();

    // Stop execution (simulates system halt after confirmed loss)
    while (true) { delay(1000); }
  }
}


// Checks whether given coordinates are within defined geofence bounds
bool checkGeofence(float lat, float lon) {
  return lat >= geofence.minLat && lat <= geofence.maxLat &&
         lon >= geofence.minLon && lon <= geofence.maxLon;
}


// Simulated alert output (would be replaced by real hardware output onto the OLED screen on the ESP32)
void sendAlert() {
  Serial.println("\n*** ALERT: Device lost! Sending alert to ESP32 screen! ***");
}


// Reads a full line from Serial input
String readLineFromSerial() {
  while (!Serial.available()) {}

  String line = Serial.readStringUntil('\n');
  line.trim();

  return line;
}


// Generates a random RSSI value within expected near-range bounds
int randomRSSI() {
  return random(rssiVeryNearMax - 5, rssiNearMax + 1);
}