#define wifi_serial Serial2
#define usb_serial Serial

boolean usb = false;
long LED_TimeOn=0;

void setup() {
  pinMode(13, OUTPUT); // Turn on LED
  digitalWrite(13, HIGH);

  // Try computer to Teensy serial
  if (!bitRead(USB0_OTGSTAT,5)) { // Determine if usb is connected
    usb = true; // Go into usb if connected to computer
    usb_serial.begin(115200);
    while (!usb_serial); // Wait for serial to connect
    usb_serial.println("USB serial connected");
  }
  
  // Setup Teensy to ESP8266 serial
  pinMode(16, INPUT);
  if (digitalRead(16)) { // Determine if wifi is on
    wifi_serial.begin(115200);
    while (!wifi_serial); // Wait for wifi serial
    if (usb) usb_serial.println("Wifi on");
    wifi_serial.println("AT+CIFSR"); // Get IP
  } else {
    if (usb) usb_serial.println("Wifi not working");
  }
  
  if (usb) usb_serial.println("Setup complete");
  
  digitalWrite(13, LOW); // Turn off LED
}

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

  // Turn off LED after a period of time
  if (millis() - LED_TimeOn > 20) {
    digitalWriteFast(13, LOW);   // set the LED off
  }
}
