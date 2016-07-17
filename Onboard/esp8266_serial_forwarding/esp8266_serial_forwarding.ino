#define wifi_serial Serial2
#define usb_serial Serial

boolean usb_connected = false;
long LED_TimeOn=0;

void setup() {
  pinMode(13, OUTPUT); // Turn on LED
  digitalWrite(13, HIGH);

  // Try computer to Teensy serial
  if (!bitRead(USB0_OTGSTAT,5)) { // Determine if usb_serial is connected
    usb_connected = true; // Debug to usb if connected to computer
    usb_serial.begin(115200);
    while (!usb_serial); // Wait for serial to connect
    usb_serial.println("Usb serial started");
  }
  
  // Setup Teensy to ESP8266 serial and wifi forwarding
  pinMode(16, INPUT);
  if (digitalRead(16)) { // Determine if wifi is on
    delay(4000); // Allow time to connect to network (WOULD LIKE TO REMOVE)
    wifi_serial.begin(115200);
    while (!wifi_serial); // Wait for wifi serial

    wifi_serial.print("+++".trim()); // End previous tranmission (DOESNT WORK EITHER)
    wifi_serial.println("ATE0"); // Disable echo
    wifi_serial.readStringUntil('OK'); // Wait for confirmation
    wifi_serial.println("AT+CIPMODE=1"); // Change transmission mode
    wifi_serial.readStringUntil('OK'); // Wait for confirmation
    if (usb_connected) usb_serial.println("Wifi configured");
    
    wifi_serial.println("AT+CIPSTATUS"); // Request connection status
    String connection_response = wifi_serial.readStringUntil('OK'); // Get connection status
    String connection_status = connection_response.charAt(connection_response.indexOf('STATUS:') + 1); // Extract status number
    if (usb_connected) usb_serial.println("Wifi status: " + connection_status);

    if (connection_status == 1 || connection_status == 5) { // Wifi is disconnected from lan
      if (usb_connected) usb_serial.println("Connecting to network...");
      // For some reason leaving a delay here does not allow the wifi to connect?
      wifi_serial.setTimeout(10000); // Allow 10 seconds
      wifi_serial.find("CONNECTED".trim()); // THIS DOESNT WORK
    }
    
    while ((connection_status == 2 || connection_status == 4) && !wifi_serial.findUntil("OK","ERROR")) { // Wifi is not connected to server
      if (usb_connected) usb_serial.println("Connecting to server...");
      wifi_serial.println("AT+CIPSTART=\"TCP\",\"192.168.0.2\",9999"); // Connect to server
      delay(2000);
    }
    if (usb_connected) usb_serial.println("Wifi connected to server");

    wifi_serial.println("AT+CIPSEND"); // Start transmission
    wifi_serial.readStringUntil('>'); // Wait for confirmation, all data sent to wifi serial is now forwarded
    if (usb_connected) usb_serial.println("Wifi setup complete");
    wifi_serial.println("test packet pls ignore"); // Test connection
  } else {
    if (usb_connected) usb_serial.println("Wifi not working");
  }
  
  if (usb_connected) usb_serial.println("Setup finished");
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
