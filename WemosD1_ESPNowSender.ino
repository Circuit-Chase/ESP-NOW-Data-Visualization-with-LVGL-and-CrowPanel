/************************************************
 * WemosD1_ESPNowSender.ino
 * Sends random values + a device ID to an ESP32-S3.
 ************************************************/
#include <ESP8266WiFi.h>
#include <espnow.h>

// Change this to uniquely identify each Wemos (1 to 4)
#define WEMOS_ID 1

// MAC address of the ESP32-S3 receiver in station mode
uint8_t RECEIVER_MAC[] = {0x48, 0x27, 0xE2, 0x8D, 0x10, 0x38};

// Data structure to send
typedef struct {
  uint8_t deviceId;
  int     randomValue;
} DataPackage;

DataPackage myData;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);  // Must be in station mode for ESP-NOW
  
  // Print MAC Address of this Wemos (for reference)
  Serial.print("This Wemos MAC: ");
  Serial.println(WiFi.macAddress());

  pinMode(D4, OUTPUT);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register send callback
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);

  // Register peer (the ESP32-S3)
  esp_now_add_peer(RECEIVER_MAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  Serial.println("Wemos Sender ready...");
}

void loop() {
  static unsigned long lastSend = 0;
  unsigned long now = millis();
  
  // Send new random value every 5 seconds
  if (now - lastSend > 1000) {
    lastSend = now;
    myData.deviceId = WEMOS_ID;
    myData.randomValue = random(0, 1000);
    
    // Send data
    esp_now_send(RECEIVER_MAC, (uint8_t *)&myData, sizeof(myData));
    Serial.print("Sent => deviceId: ");
    Serial.print(myData.deviceId);
    Serial.print("  randomValue: ");
    Serial.println(myData.randomValue);
    digitalWrite(D4, LOW);
    delay(100);
    digitalWrite(D4, HIGH);
  }
}
