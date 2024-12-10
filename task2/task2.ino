#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

#define red 4
#define ylw 19
#define grn 5
#define blu 22
#define buttonPin 33

const char* ssid = "Redmi 110A";           // Your Wi-Fi SSID
const char* password = "99999999";   // Your Wi-Fi Password

const char* firmwareUrl = "https://raw.githubusercontent.com/ttpu/lab7-TheDarkLord777/refs/heads/main/task2/data.bin";  // URL to the .bin file
const float currentVersion = 1.0;    // Current firmware version
const char* versionUrl = "https://raw.githubusercontent.com/ttpu/lab7-TheDarkLord777/refs/heads/main/task2/version";  // URL to version text file

unsigned long previousMillisRed = 0; 
unsigned long previousMillisGreen = 0;
unsigned long previousMillisYellow = 0; 
unsigned long previousMillisBlue = 0;

const long intervalRed = 1000;  
const long intervalGreen = 500;  
const long intervalYellow = 3000; 
const long intervalBlue = 5000; 


int redState = LOW;
int greenState = LOW;
int yellowState = LOW;
int blueState = LOW;

// Variables for button state detection
bool lastButtonState = LOW;  // The previous state of the button
unsigned long lastPressTime = 0; // Time when the button state last changed

void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  pinMode(red, OUTPUT);
  pinMode(grn, OUTPUT);
  pinMode(blu, OUTPUT);
  pinMode(ylw, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);  // Use internal pull-up resistor

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");

  // Check for updates on startup
  checkForUpdates();
}

void loop() {
  // Check for firmware updates periodically
  static unsigned long lastCheck = 0;
  const unsigned long interval = 10000;  // Check every 10 seconds
  if (millis() - lastCheck > interval) {
    lastCheck = millis();
    checkForUpdates();
  }

  // Read the button state
  bool currentButtonState = digitalRead(buttonPin) == LOW;  // Active LOW button
  
  // Detect button press or release
  if (currentButtonState != lastButtonState) {
    lastPressTime = millis();
    if (currentButtonState) {
      // Button was pressed
      Serial.println("Button pressed, starting LED blink...");
    } else {
      // Button was released
      Serial.println("Button released.");
    }
    lastButtonState = currentButtonState;
  }

  // If button is pressed, blink LEDs
  if (currentButtonState) {
    blinkLEDs();
  }
}

// Function to check for firmware updates
void checkForUpdates() {
  Serial.println("Checking for firmware updates...");

  HTTPClient http;

  // Fetch the latest version number from the server
  http.begin(versionUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String newVersionStr = http.getString();
    newVersionStr.trim();  // Remove any whitespace or newline characters
    float newVersion = newVersionStr.toFloat();

    Serial.print("Current firmware version: ");
    Serial.println(currentVersion, 2);
    Serial.print("Available firmware version: ");
    Serial.println(newVersion, 2);

    if (newVersion > currentVersion) {
      Serial.println("New firmware available. Starting update...");
      http.end();  // Close the connection before proceeding
      downloadAndUpdate();
    } else {
      Serial.println("Already on the latest version.");
    }
  } else {
    Serial.print("Failed to check for updates. HTTP error code: ");
    Serial.println(httpCode);
  }
  http.end();
}

// Function to download and apply the firmware update
void downloadAndUpdate() {
  HTTPClient http;

  // Begin downloading the new firmware
  http.begin(firmwareUrl);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    int contentLength = http.getSize();

    if (contentLength > 0) {
      bool canBegin = Update.begin(contentLength);

      if (canBegin) {
        Serial.println("Beginning firmware update...");

        WiFiClient* client = http.getStreamPtr();
        size_t written = Update.writeStream(*client);

        if (written == contentLength) {
          Serial.println("Firmware update successfully written.");
        } else {
          Serial.print("Only ");
          Serial.print(written);
          Serial.print(" out of ");
          Serial.print(contentLength);
          Serial.println(" bytes were written. Update failed.");
          http.end();
          return;
        }

        if (Update.end()) {
          Serial.println("Update finished successfully.");
          if (Update.isFinished()) {
            Serial.println("Update successfully completed. Rebooting...");
            ESP.restart();
          } else {
            Serial.println("Update did not complete. Something went wrong.");
          }
        } else {
          Serial.print("Error Occurred. Error #: ");
          Serial.println(Update.getError());
        }
      } else {
        Serial.println("Not enough space to begin OTA update.");
      }
    } else {
      Serial.println("Content length is not valid.");
    }
  } else {
    Serial.print("Failed to download firmware. HTTP error code: ");
    Serial.println(httpCode);
  }
  http.end();
}


void blinkLEDs() {
  static int redBlinkCount = 0;
  static int greenBlinkCount = 0;
  static int yellowBlinkCount = 0;
  static int blueBlinkCount = 0;

  unsigned long currentMillis = millis();

  // Red LED: Blink 5 times every 1 second
  if (redBlinkCount < 5 && currentMillis - previousMillisRed >= 1000) {
    previousMillisRed = currentMillis;
    redState = (redState == LOW) ? HIGH : LOW;
    digitalWrite(red, redState);
    if (redState == LOW) redBlinkCount++;
  }

  // Green LED: Blink 10 times every 0.5 second
  if (greenBlinkCount < 10 && currentMillis - previousMillisGreen >= 500) {
    previousMillisGreen = currentMillis;
    greenState = (greenState == LOW) ? HIGH : LOW;
    digitalWrite(grn, greenState);
    if (greenState == LOW) greenBlinkCount++;
  }

  // Yellow LED: Blink 3 times every 3 seconds
  if (yellowBlinkCount < 3 && currentMillis - previousMillisYellow >= 3000) {
    previousMillisYellow = currentMillis;
    yellowState = (yellowState == LOW) ? HIGH : LOW;
    digitalWrite(ylw, yellowState);
    if (yellowState == LOW) yellowBlinkCount++;
  }

  // Blue LED: Blink 2 times every 5 seconds
  if (blueBlinkCount < 2 && currentMillis - previousMillisBlue >= 5000) {
    previousMillisBlue = currentMillis;
    blueState = (blueState == LOW) ? HIGH : LOW;
    digitalWrite(blu, blueState);
    if (blueState == LOW) blueBlinkCount++;
  }

  // Reset states after blinking
  if (redBlinkCount >= 5 && greenBlinkCount >= 10 && yellowBlinkCount >= 3 && blueBlinkCount >= 2) {
    redState = LOW;
    greenState = LOW;
    yellowState = LOW;
    blueState = LOW;
    digitalWrite(red, redState);
    digitalWrite(grn, greenState);
    digitalWrite(ylw, yellowState);
    digitalWrite(blu, blueState);
    // Reset blink counts
    redBlinkCount = 0;
    greenBlinkCount = 0;
    yellowBlinkCount = 0;
    blueBlinkCount = 0;
  }
}

