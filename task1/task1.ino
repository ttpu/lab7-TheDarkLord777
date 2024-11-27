#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define red 4
#define ylw 19
#define grn 5
#define blu 22

const char* ssid = "Bomish";          // Replace with your Wi-Fi SSID
const char* password = "Yolgizim";  // Replace with your Wi-Fi Password

unsigned long previousMillisRed = 0; 
unsigned long previousMillisGreen = 0;
unsigned long previousMillisYellow = 0; 
unsigned long previousMillisBlue = 0; 

const long intervalRed = 1000;  
const long intervalGreen = 500;  
const long intervalYellow = 3000; 
const long intervalBlue = 5000;  

void setup() {
  Serial.begin(115200);
  
  Serial.println("Booting...");
  pinMode(red,OUTPUT);
  pinMode(grn,OUTPUT);
  pinMode(blu,OUTPUT);
  pinMode(ylw,OUTPUT);
  
  // Connect to your Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for the connection to establish
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Configure OTA
  ArduinoOTA.setHostname("esp32-ota");   // Optional: Set a custom hostname
  // ArduinoOTA.setPassword("admin");    // Optional: Set an OTA password

  // OTA Event Handlers (Optional but recommended for debugging)
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating...");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nUpdate Complete!");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress * 100) / total);
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error [%u]: ", error);
    if      (error == OTA_AUTH_ERROR)    Serial.println("Authentication Failed");
    else if (error == OTA_BEGIN_ERROR)   Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connection Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)     Serial.println("End Failed");

    
  });

  // Start the OTA service
  ArduinoOTA.begin();

  Serial.println("Ready for OTA updates.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // Handle OTA updates
  ArduinoOTA.handle();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisRed >= intervalRed) {
    previousMillisRed = currentMillis;
    digitalWrite(red, !digitalRead(red));  
  }

  if (currentMillis - previousMillisGreen >= intervalGreen) {
    previousMillisGreen = currentMillis;
    digitalWrite(grn, !digitalRead(grn)); 
  }

   if (currentMillis - previousMillisYellow >= intervalYellow) {
    previousMillisYellow = currentMillis;
    digitalWrite(ylw, !digitalRead(ylw));  
  }
  if (currentMillis - previousMillisBlue >= intervalBlue) {
    previousMillisBlue = currentMillis;
    digitalWrite(blu, !digitalRead(blu)); 
  }
}
