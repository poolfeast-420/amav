//**** Using a Teensy 3.1 to Connect an ESP8266 to PC USB Serial *******
//     Compiled with Arduino 1.60 and Teensyduino 1.21b6
//     ESP8266 Firmware: AT21SDK95-2015-01-24.bin

#define wifi_serial Serial2
#define usb_serial Serial

void setup() {
    pinMode(13, OUTPUT); // Turn on LED
    digitalWrite(13, HIGH);

    // Setup computer to Teensy serial
    usb_serial.begin(115200);

    // Setup Teensy to ESP8266 serial
    wifi_serial.begin(115200);

    //delay(800); // Wait for computer serial

    usb_serial.println("Setup complete");
    
    digitalWrite(13, LOW); // Turn off LED
}

long LED_TimeOn=0;

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

    if (millis() - LED_TimeOn > 20) {
  digitalWriteFast(13, LOW);   // set the LED off
  }

}
