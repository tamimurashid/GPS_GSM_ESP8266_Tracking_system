#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>

// Define GPS baud rate and RX/TX pins
static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// Create a TinyGPSPlus object
TinyGPSPlus gps;

// Set up a SoftwareSerial connection for GPS
SoftwareSerial ss(RXPin, TXPin);

// Variables for timing
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 1000; // Update every 1 second

void setup() {
  // Start Serial communication for debugging
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.println("GPS Latitude and Longitude Example");
}

void loop() {
  while (ss.available() > 0) {
    gps.encode(ss.read());  // Feed the GPS data into the gps object
  }

  // Check if it's time to update the location
  if (millis() - lastUpdateTime >= updateInterval) {
    if (gps.location.isUpdated()) {  // Only print when a new location is received
      // Print the Latitude and Longitude
      double latitude = gps.location.lat();
      double longitude = gps.location.lng();
      
      Serial.print("Latitude: ");
      Serial.print(latitude, 6);  // 6 decimal places
      Serial.print(" Longitude: ");
      Serial.println(longitude, 6);  // 6 decimal places
      
      // Create a Google Maps link with the current coordinates
      String mapLink = "https://www.google.com/maps?q=" + String(latitude, 6) + "," + String(longitude, 6);
      Serial.print("Map Link: ");
      Serial.println(mapLink);  // Display the Google Maps link
    }

    // Update the last update time
    lastUpdateTime = millis();
  }
}
