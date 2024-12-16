#include <WiFi.h> 
#include <HTTPClient.h> 
#include <Update.h> 
 
const char* ssid = "Redmi 110A";           // Your Wi-Fi SSID
const char* password = "99999999";   // Your Wi-Fi Password

const char* firmwareUrl = "https://ttpu.github.io/lab7-TheDarkLord777/task2/data.bin";  // URL to the .bin file
const float currentVersion = 1.2;    // Current firmware version
const char* versionUrl = "https://ttpu.github.io/lab7-TheDarkLord777/task2/version";  // URL to version text file

 
const int buttonPin = 33; 
const int redLED = 4; 
const int greenLED = 5; 
const int yellowLED = 19; 
const int blueLED = 22; 
 
bool buttonPressed = false; 
bool blinking = false; 
unsigned long lastDebounceTime = 0; 
const unsigned long debounceDelay = 50; 
 
unsigned long prevRedTime = 0, prevGreenTime = 0, prevYellowTime = 0, prevBlueTime = 0; 
const unsigned long redInterval = 1000; 
const unsigned long greenInterval = 500; 
const unsigned long yellowInterval = 3000; 
const unsigned long blueInterval = 5000; 
int redBlinkCount = 0, greenBlinkCount = 0, yellowBlinkCount = 0, blueBlinkCount = 0; 
 
void setup() { 
  Serial.begin(115200); 
  Serial.println("Booting..."); 
 
  pinMode(buttonPin, INPUT_PULLUP); 
  pinMode(redLED, OUTPUT); 
  pinMode(greenLED, OUTPUT); 
  pinMode(yellowLED, OUTPUT); 
  pinMode(blueLED, OUTPUT); 
 
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  } 
  Serial.println("\nConnected to WiFi"); 
 
  checkForUpdates(); 
} 
 
void loop() { 
  checkButton(); 
 
  if (blinking) { 
    blinkLEDs(); 
  } 
 
  static unsigned long lastCheck = 0; 
  const unsigned long interval = 10000; 
 
  if (millis() - lastCheck > interval) { 
    lastCheck = millis(); 
    checkForUpdates(); 
  } 
} 
 
void checkButton() { 
  int buttonState = digitalRead(buttonPin); 
   
  if (buttonState == LOW && millis() - lastDebounceTime > debounceDelay) { 
    lastDebounceTime = millis(); 
    buttonPressed = true; 
  } 
 
  if (buttonPressed && buttonState == HIGH) { 
    Serial.println("Button released! Starting LED blink..."); 
    buttonPressed = false; 
 
    blinking = true; 
    redBlinkCount = 0; 
    greenBlinkCount = 0; 
    yellowBlinkCount = 0; 
    blueBlinkCount = 0; 
    prevRedTime = millis(); 
    prevGreenTime = millis(); 
    prevYellowTime = millis(); 
    prevBlueTime = millis(); 
  } 
} 
 
void blinkLEDs() { 
  unsigned long currentMillis = millis(); 
 
  // Red LED blinking 
  if (redBlinkCount < 5*2 && currentMillis - prevRedTime >= redInterval) { 
    prevRedTime = currentMillis; 
    digitalWrite(redLED, !digitalRead(redLED)); // Toggle LED 
    redBlinkCount++; 
    if (redBlinkCount == 5*2) digitalWrite(redLED, LOW); // Ensure it's off after finishing 
  } 
 
  // Green LED blinking 
  if (greenBlinkCount < 10*2 && currentMillis - prevGreenTime >= greenInterval) { 
    prevGreenTime = currentMillis; 
    digitalWrite(greenLED, !digitalRead(greenLED)); // Toggle LED 
    greenBlinkCount++; 
    if (greenBlinkCount == 10*2) digitalWrite(greenLED, LOW); // Ensure it's off after finishing 
  } 
 
  // Yellow LED blinking 
  if (yellowBlinkCount < 3*2 && currentMillis - prevYellowTime >= yellowInterval) { 
    prevYellowTime = currentMillis; 
    digitalWrite(yellowLED, !digitalRead(yellowLED)); // Toggle LED 
    yellowBlinkCount++; 
    if (yellowBlinkCount == 3*2) digitalWrite(yellowLED, LOW); // Ensure it's off after finishing 
  } 
 
  // Blue LED blinking 
  if (blueBlinkCount < 2*2 && currentMillis - prevBlueTime >= blueInterval) { 
    prevBlueTime = currentMillis; 
    digitalWrite(blueLED, !digitalRead(blueLED)); // Toggle LED 
    blueBlinkCount++; 
    if (blueBlinkCount == 2*2) digitalWrite(blueLED, LOW); // Ensure it's off after finishing 
  } 
 
  // Check if all LEDs have completed blinking 
  if (redBlinkCount >= 5*2 && greenBlinkCount >= 10*2 && yellowBlinkCount >= 3*2 && blueBlinkCount >= 2*2) { 
    blinking = false; 
    digitalWrite(redLED, LOW); 
    digitalWrite(greenLED, LOW); 
    digitalWrite(yellowLED, LOW); 
    digitalWrite(blueLED, LOW); 
    Serial.println("Blinking finished."); 
  } 
} 
 
 
void checkForUpdates() { 
  Serial.println("Checking for firmware updates..."); 
  HTTPClient http; 
 
  http.begin(versionUrl); 
  int httpCode = http.GET(); 
 
  if (httpCode == HTTP_CODE_OK) { 
    String newVersionStr = http.getString(); 
    newVersionStr.trim(); 
    float newVersion = newVersionStr.toFloat(); 
 
    Serial.print("Current firmware version: "); 
    Serial.println(currentVersion, 2); 
    Serial.print("Available firmware version: "); 
    Serial.println(newVersion, 2); 
 
    if (newVersion > currentVersion) { 
      Serial.println("New firmware available. Starting update..."); 
      http.end(); 
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
 
void downloadAndUpdate() { 
  HTTPClient http; 
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
          Serial.println("Update failed."); 
          return; 
        } 
 
        if (Update.end()) { 
          Serial.println("Update finished successfully. Rebooting..."); 
          ESP.restart(); 
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
