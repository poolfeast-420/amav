#define wifi_serial Serial2
#define usb_serial Serial

// Format message with header that describes its size
void wifi_encode_and_send(String message) {
    wifi_serial.printf("%04d", message.length());
    wifi_serial.print(message);
}

// Decode a formated message with header that describes its size
String wifi_decode_and_recieve() {
  char *buffer[4];
  wifi_serial.readBytes(buffer[0], 4);
  log(buffer[0]);
  // char *output[9999];
  // wifi_serial.readBytes(output[0], buffer[0]);
 }

// Called if something goes wrong
void error() {
  log("oh no"); // give up
  while (true) { // Blink led
    digitalWrite(13, HIGH);   // set the LED on
    delay(600);
    digitalWrite(13, LOW);    // set the LED off
    delay(600);
  }
}

// Record message to preferably USB but otherwise wifi
void log(String text) {
  if (!bitRead(USB0_OTGSTAT, 5)) { // Determine if usb is connected
    if (usb_serial) {
      usb_serial.println(text);
    } else {
      usb_serial.begin(115200); // Begin usb connection
      while (!usb_serial); // Wait for serial to connect
      usb_serial.println(text);
    }
  } else { // Not much can be done to setup wifi, just have to hope
    if (wifi_serial) {
      wifi_serial.println(text);
    } else {
      wifi_serial.begin(115200); // Begin usb connection
      while (!wifi_serial); // Wait for serial to connect
      wifi_serial.println(text);
    }
  }
}

void setup() {
  pinMode(13, OUTPUT); // Turn on LED
  digitalWrite(13, HIGH);

  // Try computer to Teensy serial
  if (!bitRead(USB0_OTGSTAT, 5)) { // Determine if usb_serial is connected
    usb_serial.begin(115200);
    while (!usb_serial); // Wait for serial to connect
    log("Usb serial started");
  }
  
  // Setup Teensy to ESP8266 serial and wifi forwarding
  pinMode(16, INPUT);
  if (digitalRead(16)) { // Determine if wifi is on
    log("Waiting for wlan to connect");
    delay(6000); // Allow time to connect to network (only works here) (WOULD LIKE TO REMOVE)
    wifi_serial.begin(115200);
    while (!wifi_serial); // Wait for wifi serial
    
    // Send parameters
    wifi_serial.write('+++'); // End previous tranmission (DOESNT WORK EITHER)
    wifi_serial.println("ATE0"); // Disable echo
    if (!wifi_serial.findUntil("OK", "ERROR")) error(); // Wait for confirmation
    wifi_serial.println("AT+CIPMODE=1"); // Change transmission mode
    if (!wifi_serial.findUntil("OK", "ERROR")) error(); // Wait for confirmation
    log("Wifi configured");

    // Get current connection status
    wifi_serial.println("AT+CIPSTATUS"); // Request connection status
    String connection_response = wifi_serial.readStringUntil('OK'); // recieve connection status
    String connection_status = connection_response.charAt(connection_response.indexOf('STATUS:') + 1); // Extract status number
    log("Wifi status: " + connection_status);

    // Connect wifi to network if not already
    if (connection_status == 1 || connection_status == 5) { // Wifi is disconnected from LAN
      log("Connecting to network...");
      // For some reason leaving a delay here does not allow the wifi to connect?
      if (!wifi_serial.find("WIFI GOT IP")) error(); // THIS DOESNT WORK
    }
    log("Wifi connected to network");

    // Connect wifi to server if not already
    while ((connection_status == 2 || connection_status == 4) && !wifi_serial.findUntil("OK", "ERROR")) { // Wifi is not connected to server
      log("Connecting to server...");
      wifi_serial.println("AT+CIPSTART=\"TCP\",\"192.168.0.4\",8080"); // Connect to server
      delay(2000);
    }
    log("Wifi connected to server");
    
    // Tell wifi to continuously send all recieved serial
    wifi_serial.println("AT+CIPSEND"); // Start transmission
    if (!wifi_serial.findUntil(">", "ERROR")) error(); // Wait for confirmation
    
    log("Wifi setup complete");
    wifi_encode_and_send("test packet pls ignore"); // Test connection
    delay(1000);
    wifi_encode_and_send("test packet ignore if you want");
    delay(2000);
    wifi_encode_and_send("test packet <-- this is a lie");
    delay(3000);
    wifi_encode_and_send("real packet please take note");

  }
  else { // Wifi is not on/attached
    error();
  }

  log("Setup finished");
  digitalWrite(13, LOW); // Turn off LED
}

long LED_TimeOn = 0;

void loop() {
  // Send bytes from ESP8266 -> Teensy to Computer
  if ( wifi_serial.available() ) {
    digitalWriteFast(13, HIGH);   // set the LED on
    LED_TimeOn = millis();
    usb_serial.write( wifi_decode_and_recieve() );
  }

  // Send bytes from Computer -> Teensy back to ESP8266
  if ( usb_serial.available() ) {
    digitalWriteFast(13, HIGH);   // set the LED on
    LED_TimeOn = millis();
    wifi_encode_and_send( usb_serial.read() ); // NOT WORKING ALSO MORE DATATYPE STUFF
  }

  // Turn off LED after a period of time
  if (millis() - LED_TimeOn > 15) {
    digitalWriteFast(13, LOW);   // set the LED off
  }
}
