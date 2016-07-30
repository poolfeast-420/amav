#define wifi_serial Serial2
#define usb_serial Serial

void setup() {
  pinMode(13, OUTPUT);
  while (!bitRead(USB0_OTGSTAT, 5)); // Wait for usb connection
  usb_serial.begin(250000);
  while (!usb_serial); // Wait for serial to connect
  usb_serial.println("Usb serial started");
  wifi_serial.begin(115200);
  while (!wifi_serial);
}

long LED_TimeOn = 0;

void loop() {
  // Send bytes from ESP8266 -> Teensy to Computer
  if ( wifi_serial.available() ) {
    digitalWriteFast(13, HIGH);   // set the LED on
    LED_TimeOn = millis();
    usb_serial.write( wifi_serial.read() );
  }

  // Send bytes from Computer -> Teensy back to ESP8266
  if ( usb_serial.available() ) {
    digitalWriteFast(13, HIGH);   // set the LED on
    LED_TimeOn = millis();
    wifi_serial.write( usb_serial.read() );
  }

  // Led flashing
  if (millis() - LED_TimeOn > 10) {
    digitalWriteFast(13, LOW);   // set the LED off
  }
}
