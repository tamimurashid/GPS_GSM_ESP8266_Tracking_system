#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// Define GPS baud rate and RX/TX pins
static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// Create a TinyGPSPlus object
TinyGPSPlus gps;

// Set up a SoftwareSerial connection for GSM (RX, TX for the GSM module)
SoftwareSerial gsmSerial(12, 13);  // RX, TX pins for GSM

// Set up a SoftwareSerial connection for GPS (using RXPin and TXPin)
SoftwareSerial ss(RXPin, TXPin);

// Replace with the phone number you want to send SMS to
const char* phoneNumber = "+255768857064";  // Example phone number

// Variables for timing
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 1000; // Update every 1 second

// Function prototype
void sendSMS(String message);

void setup() {
  // Start Serial communication for debugging
  Serial.begin(115200);
  ss.begin(GPSBaud);
  gsmSerial.begin(9600);  // Set the GSM baud rate
  
  // Give the GSM module some time to register to the network
  delay(1000);

  // Set the GSM module to SMS mode
  gsmSerial.println("AT+CMGF=1");  // Set to SMS text mode
  delay(1000);  // Wait for the command to be executed

  Serial.println("GPS and GSM Setup Complete");
}

void loop() {
  // Check if the GSM module has received any data
  if (gsmSerial.available()) {
    String sms = "";
    while (gsmSerial.available()) {
      sms += (char)gsmSerial.read();  // Read incoming SMS data
    }

    // If the received SMS contains the word "location", send the Google Maps link
    if (sms.indexOf("location") != -1) {
      // Retrieve GPS coordinates
      double latitude = gps.location.lat();
      double longitude = gps.location.lng();
      
      // Create a Google Maps link with the current coordinates
      String mapLink = "https://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);
      
      // Send the Google Maps link via SMS
      sendSMS(mapLink);
    }
  }

  // Update GPS coordinates periodically
  while (ss.available() > 0) {
    gps.encode(ss.read());  // Feed the GPS data into the gps object
  }

  // Check if it's time to update the location
  if (millis() - lastUpdateTime >= updateInterval) {
    if (gps.location.isUpdated()) {  // Only print when a new location is received
      // Print the Latitude and Longitude to the Serial Monitor
      double latitude = gps.location.lat();
      double longitude = gps.location.lng();
      
      Serial.print("Latitude: ");
      Serial.print(latitude, 6);  // 6 decimal places
      Serial.print(" Longitude: ");
      Serial.println(longitude, 6);  // 6 decimal places
    }

    // Update the last update time
    lastUpdateTime = millis();
  }
}

// Function to send SMS using the GSM module
void sendSMS(String message) {
  gsmSerial.println("AT+CMGS=\"" + String(phoneNumber) + "\"");  // Send SMS to the number
  delay(1000);
  
  gsmSerial.print(message);  // Message body
  delay(100);
  
  gsmSerial.write(26);  // Send the "Ctrl+Z" character to indicate end of message
  delay(5000);  // Wait for the message to be sent
  Serial.println("SMS Sent: " + message);
}
