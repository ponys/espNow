#include <Arduino.h>
#include <DHT.h>
#include <Wifi.h>
#include <esp_now.h>

#define DHT11_PIN 14
#define DHTTYPE DHT11
#define CHANNEL 1

DHT dht(DHT11_PIN, DHTTYPE);

esp_now_peer_info_t slave;
uint8_t macSlave[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // SLAVE STA MAC ADDRESS

struct sensorData {
    float humidity;
    float temperature;
};

void sendData(float h, float t) {
  sensorData sd;
  sd.humidity = h;
  sd.temperature = t;

  uint8_t data[sizeof(sd)];
  memcpy(data, &sd, sizeof(sd));

  esp_err_t result = esp_now_send(slave.peer_addr, data, sizeof(data));
  Serial.print("*** SEND STATUS: ");
  if (result == ESP_OK) {
    Serial.println("Success ***");
  } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init. ***");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument ***");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error ***");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM ***");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found. ***");
  } else {
    Serial.println("Not sure what happened ***");
  }
}

void InitESPNow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println( "*** INITIALIZING ESP-NOW ***");
        ESP.restart();
        delay(10);
    }
}

bool registerSlave() {
    Serial.println("*** REGISTERING SLAVE ***");
    const esp_now_peer_info_t *peer = &slave;
    esp_err_t addStatus = esp_now_add_peer(peer);
    if (addStatus == ESP_OK) {
        Serial.println("Pair success");
        return true;
    } else {
        Serial.println("Pair not registered");
        return false;
    }
}


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "SUCCESS" : "FAILED");
}

void setup() {
    Serial.begin(9600);
    dht.begin();
    
    WiFi.mode(WIFI_MODE_STA);
    slave.channel = CHANNEL;
    slave.encrypt = 0;
    memcpy(slave.peer_addr, macSlave, sizeof(macSlave));
    InitESPNow();
    esp_now_register_send_cb(OnDataSent);

    while (!registerSlave()) {
        delay(1000);
    }
}

void loop() {
    delay(10000);
    float humidity = dht.readHumidity();
    float temp = dht.readTemperature();
    sendData(humidity, temp);    
}