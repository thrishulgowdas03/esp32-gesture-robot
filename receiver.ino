#include <esp_now.h>
#include <WiFi.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// Motor Pins
const int IN1 = 27, IN2 = 26;
const int IN3 = 25, IN4 = 33;
const int ENA = 14, ENB = 32;

const int FREQ       = 1000;
const int RESOLUTION = 8;
const int SPEED      = 120;

typedef struct {
  char command;
} CarCommand;

CarCommand data;

unsigned long lastReceiveTime  = 0;
unsigned long lastBTTime       = 0;
const int TIMEOUT_MS           = 1000;
const int BT_PRIORITY_MS       = 500;  // BT holds priority for 500ms after last BT command

// Tracks which source is active
bool btActive = false;

void onReceive(const esp_now_recv_info_t *info, const uint8_t *inData, int len) {
  // If BT was used recently, ignore ESP-NOW
  if (btActive) return;

  memcpy(&data, inData, sizeof(data));
  lastReceiveTime = millis();

  Serial.print("[ESP-NOW] "); Serial.println(data.command);
  executeCommand(data.command);
}

void executeCommand(char command) {
  switch (command) {
    case 'F': moveForward();  break;
    case 'B': moveBackward(); break;
    case 'L': turnLeft();     break;
    case 'R': turnRight();    break;
    case 'S': stopCar();      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  ledcAttach(ENA, FREQ, RESOLUTION);
  ledcAttach(ENB, FREQ, RESOLUTION);
  ledcWrite(ENA, SPEED);
  ledcWrite(ENB, SPEED);

  stopCar();

  SerialBT.begin("ESP32_Car_Pro");
  Serial.println("Bluetooth Ready!");

  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);

  Serial.println("Car Ready!");
}

void loop() {
  // ── Update BT priority window ──
  if (millis() - lastBTTime > BT_PRIORITY_MS) {
    btActive = false;  // BT priority expired, ESP-NOW can take over
  }

  // ── Bluetooth Commands ──
  if (SerialBT.available()) {
    char command = SerialBT.read();

    // Ignore noise — only valid commands
    if (command == 'F' || command == 'B' ||
        command == 'L' || command == 'R' || command == 'S') {

      btActive      = true;
      lastBTTime    = millis();
      lastReceiveTime = millis();  // Reset timeout

      Serial.print("[BT] "); Serial.println(command);
      executeCommand(command);
    }
  }

  // ── Safety Timeout ──
  if (millis() - lastReceiveTime > TIMEOUT_MS) {
    stopCar();
  }
}

// ─── Motor Functions ────────────────────────────────────────────
void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void moveBackward() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}
void turnLeft() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}
void turnRight() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void stopCar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}