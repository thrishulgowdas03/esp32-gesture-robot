#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;
uint8_t receiverMAC[] = {0x00, 0x70, 0x07, 0x1D, 0x67, 0xA8};

typedef struct {
  char command;
} CarCommand;

CarCommand data;
esp_now_peer_info_t peerInfo;

void onSent(const wifi_tx_info_t *txInfo, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sent OK" : "Send FAILED");
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  mpu.initialize();
  Serial.println(mpu.testConnection() ? "MPU6050 OK" : "MPU6050 FAILED");

  WiFi.mode(WIFI_STA);

  if (esp_now_init() == ESP_OK)
    Serial.println("ESP-NOW Init OK");
  else
    Serial.println("ESP-NOW Init FAILED");

  esp_now_register_send_cb(onSent);

  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) == ESP_OK)
    Serial.println("Peer Added OK");
  else
    Serial.println("Peer Add FAILED");
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  Serial.print("AX: "); Serial.print(ax);
  Serial.print(" | AY: "); Serial.print(ay);

  char command = 'S';
  if      (ax < -7000 && ay > -3000 && ay < 3000) command = 'F';
  else if (ax >  7000 && ay > -3000 && ay < 3000) command = 'B';
  else if (ay >  7000 && ax > -3000 && ax < 3000) command = 'R';
  else if (ay < -7000 && ax > -3000 && ax < 3000) command = 'L';

  Serial.print(" | CMD: "); Serial.println(command);

  data.command = command;
  esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));

  delay(100);
}