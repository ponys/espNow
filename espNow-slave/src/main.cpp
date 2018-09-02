#include <Arduino.h>
#include <Wifi.h>
#include <esp_now.h>

const char* WIFI_SSID     = "SSID";
const char* WIFI_PASSWORD = "PWD";
struct sensorData {
    float humidity;
    float temperature;
};

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

// callback
void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    Serial.println("*** DATA RECEIVED ***");
    sensorData sd;
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
}


void setup() {
    Serial.begin(9600);
    WiFi.mode(WIFI_MODE_STA); 
    InitESPNow();    
    esp_now_register_recv_cb(onDataRecv);
}

void loop() {
    // nothing
}