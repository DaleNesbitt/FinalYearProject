#define RXD2 16
#define TXD2 17

HardwareSerial AT09(2); // UART2 for the BLE module

void setup() {
  Serial.begin(115200);

  // Start UART2 for the AT-09 BLE module
  // 9600 is the default baud rate for AT-09. (Later discovered it was 15200, but keeping this for thinking at the time)
  // RXD2, TXD2 is the GPIO pins defined above. One sends data, one receives
  AT09.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.println("AT-09 BLE raw data monitor");
}

void loop() {
  // Check if there's any data coming from the BLE module
  while (AT09.available()) {

    // Read one byte from UART2
    uint8_t b = AT09.read();

    // If the value is less than 16, it means it's a single HEX digit
    // add a leading 0 so formatting stays consistent (like 0A instead of A)
    if (b < 16) Serial.print("0");

    // Print the byte in HEX format
    Serial.print(b, HEX);

    // Add a space between each byte for readability and clarity when converting
    Serial.print(" ");
  }
}