#include <Arduino.h>
#include <Wifi.h>
#include <esp_now.h>
#include <HTTPClient.h>

const char* WIFI_SSID     = "SSID";
const char* WIFI_PASSWORD = "PWD";

// Structure to handle data received from master
struct sensorData {
    float humidity;
    float temperature;
};

// define data, and a boolean to control when new data needs to be sent over http
boolean newData = false;
sensorData sd;

// Init ESP Now
void InitESPNow() {
    if (esp_now_init() != ESP_OK) {
      Serial.println( "*** INITIALIZING ESP-NOW ***");
      ESP.restart();
      delay(10);
  }
}

void connectToNetwork() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("*** CONNECTING TO WIFI ***");
        Serial.println(WiFi.status());
    }
    Serial.println("*** CONNECTED TO WIFI ***");
}

void uploadData() {
    Serial.println("*** SEND DATA OVER WIFI ***");
    HTTPClient http;
    String url = "https://api.thingspeak.com/update?api_key=APIKEY&field1=" + String(sd.humidity) + "&field2=" + String(sd.temperature);
    http.begin(url);
    http.GET();
    http.end();
    Serial.println("*** DATA SENT OVER WIFI ***");
}

// Callback function when data is received, runs on core 0
void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    Serial.println("*** DATA RECEIVED ***");
    memcpy(&sd, data, sizeof(sd));
    Serial.print("From");
    for (int i = 0 ; i < 6; i++){
      Serial.print(":");
      Serial.print(mac_addr[i], HEX);            
    }
    Serial.println("");
    Serial.print("Humidity: ");
    Serial.print(sd.humidity);
    Serial.println(" %");
    Serial.print("Temperature: ");
    Serial.print(sd.temperature);
    Serial.println(" °C");
    newData = true;
}

void setup() {
    Serial.begin(9600);
    WiFi.mode(WIFI_MODE_APSTA); 
    connectToNetwork();
    InitESPNow();    
    esp_now_register_recv_cb(onDataRecv);
}

// Since callback runs on core0, upload needs to happens on core1.
// Doing the upload on loop cause it runs on core1
void loop() {
    delay(500);
    if (newData) {
        newData = false;
        uploadData(); 
    }   
}