#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>

#define red 4
#define ylw 19
#define grn 5
#define blu 22
#define buttonPin 33

const char* ssid = "Bomish";           // Your Wi-Fi SSID
const char* password = "Yolgizim";   // Your Wi-Fi Password

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

// Variables for button state detection
bool lastButtonState = HIGH;  // The previous state of the button
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
  unsigned long currentMillis = millis();

 
  if (currentMillis - previousMillisRed >= intervalRed) {
    previousMillisRed = currentMillis;
    for (int i = 0; i < 5; i++) {
      digitalWrite(red, HIGH);
      delay(100);  // 100ms ON
      digitalWrite(red, LOW);
      delay(100);  // 100ms OFF
    }
  }


  if (currentMillis - previousMillisGreen >= intervalGreen) {
    previousMillisGreen = currentMillis;
    for (int i = 0; i < 10; i++) {
      digitalWrite(grn, HIGH);
      delay(50);   // 50ms ON
      digitalWrite(grn, LOW);
      delay(50);   // 50ms OFF
    }
  }


  if (currentMillis - previousMillisYellow >= intervalYellow) {
    previousMillisYellow = currentMillis;
    for (int i = 0; i < 3; i++) {
      digitalWrite(ylw, HIGH);
      delay(1000);  
      digitalWrite(ylw, LOW);
      delay(1000);  
    }
  }
  if (currentMillis - previousMillisBlue >= intervalBlue) {
    previousMillisBlue = currentMillis;
    for (int i = 0; i < 2; i++) {
      digitalWrite(blu, HIGH);
      delay(2500);  
      digitalWrite(blu, LOW);
      delay(2500);  
    }
  }
}
