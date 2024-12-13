

#include <TinyGPS++.h> #include <SoftwareSerial.h>

#define ALCOHOL_SENSOR A0 #define BUZZER 8
#define RELAY 9
#define GPS_RX_PIN 10
#define GPS_TX_PIN 11
#define GSM_RX_PIN 7
#define GSM_TX_PIN 6

SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN); SoftwareSerial gsmSerial(GSM_RX_PIN, GSM_TX_PIN); TinyGPSPlus gps;

int threshold = 400;
int debounceDelay = 5000; // Debounce delay set to 5 seconds int sampleCount = 10;

unsigned long lastDetectionTime = 0; bool alertSent = false;

void setup() { pinMode(ALCOHOL_SENSOR, INPUT); pinMode(BUZZER, OUTPUT); pinMode(RELAY, OUTPUT);

digitalWrite(BUZZER, LOW); digitalWrite(RELAY, HIGH);

Serial.begin(9600); gpsSerial.begin(9600); gsmSerial.begin(9600);

delay(2000); // Allow time for GSM module to initialize sendSMS("Vehicle Alcohol Detection System Initialized."); Serial.println("System Initialized.");
}

void loop() {
int alcoholLevel = getAverageAlcoholLevel();
Serial.print("Average Alcohol Level: "); Serial.println(alcoholLevel);

if (alcoholLevel > threshold) {
if (!alertSent || (millis() - lastDetectionTime > debounceDelay)) { handleAlert();
lastDetectionTime = millis(); alertSent = true;
}
} else { resetSystem();
}

delay(1000);
}

int getAverageAlcoholLevel() { int total = 0;
for (int i = 0; i < sampleCount; i++) { total += analogRead(ALCOHOL_SENSOR); delay(10);
}
return total / sampleCount;
}

void handleAlert() { digitalWrite(BUZZER, HIGH); digitalWrite(RELAY, LOW);

// Send SMS immediately without waiting for GPS
String alertMessage = "WARNING: Driver is intoxicated! Vehicle stopped."; sendSMS(alertMessage);

Serial.println(alertMessage);

// Attempt to send GPS data if available if (getGPSLocation()) {
String locationMessage = "Location: Latitude: " + String(gps.location.lat(), 6) +
", Longitude: " + String(gps.location.lng(), 6); sendSMS("Driver intoxicated! " + locationMessage); Serial.println("Location details sent: " + locationMessage);
} else {
Serial.println("GPS LOCATION NOT FOUND");
}
}

bool getGPSLocation() {
// Read data from GPS serial while (gpsSerial.available()) {
gps.encode(gpsSerial.read());
}
// Return true if valid location is updated, otherwise false return gps.location.isValid() && gps.location.isUpdated();
}

void resetSystem() { digitalWrite(BUZZER, LOW); digitalWrite(RELAY, HIGH); alertSent = false;
}

void sendSMS(String message) {
gsmSerial.println("AT+CMGF=1"); // Set GSM module to text mode delay(500);
gsmSerial.println("AT+CMGS=\"+91 YOUR NUMBER\""); // Replace with your phone number
delay(500); gsmSerial.print(message);
gsmSerial.write(26); // End of message character (CTRL+Z) delay(500);
}
