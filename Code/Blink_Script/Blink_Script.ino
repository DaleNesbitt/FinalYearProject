// LED in GPIO pin 4
#define LED 4

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  // Blink LED light on for 1 second (1000)
  digitalWrite(LED, HIGH);
  delay(1000);
  // Blink LED off for 1 second
  digitalWrite(LED, LOW);
  delay(1000);
}
