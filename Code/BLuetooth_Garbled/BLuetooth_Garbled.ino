#define RXD2 16
#define TXD2 17

// Create UART2 interface for communication with AT-09 BLE module
HardwareSerial AT09(2);

void setup() {
  // Start Serial Monitor for debugging
  Serial.begin(115200);

  // Wait for Serial Monitor to initialise
  while(!Serial);

  // Header
  Serial.println("=== AT-09 BLE Test ===");

  // Initialise UART2 for BLE module communication
  // AT-09 default baud rate is 9600 (incorrect. Later corrected)
  AT09.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void loop() {
  // Continuously check if data is available from BLE module
  while(AT09.available()) {

    // Read incoming data as a character (ASCII assumption)
    char c = AT09.read();

    // Write directly to Serial Monitor
    // This assumes the incoming data is readable text - later discovered it was not
    Serial.write(c);
  }
}