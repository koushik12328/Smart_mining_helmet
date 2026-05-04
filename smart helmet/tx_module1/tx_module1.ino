#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <Wire.h>

// --- PIN CONFIGURATION ---
#define MQ2_PIN 34
#define MQ4_PIN 32
#define MQ135_PIN 35
#define DHT_PIN 4
#define BUZZER_PIN 13
#define GREEN_LED 2
#define RED_LED 15
#define I2C_SDA 21
#define I2C_SCL 22

// --- SENSOR THRESHOLDS ---
// --- GAS SENSORS (12-bit ADC: 0-4095) ---
const int MQ2_LIMIT    = 2500;   // Smoke/Combustible
const int MQ4_LIMIT    = 2200;   // Methane
const int MQ135_LIMIT  = 1800;   // Air Quality (Threshold for poor air)
const float TEMP_LIMIT = 48.0;  // Critical Heat
const float HUM_HIGH   = 85.0;  // Condensation Risk
const float HUM_LOW    = 20.0;  // ESD Risk
const long PRESS_MIN   = 98000; // adjusted according to mine
// --- COMMUNICATION ---
uint8_t broadcastAddress[] = {0xB4, 0xBF, 0xE9, 0x0E, 0xEF, 0xC4};

typedef struct struct_message {
    float temp;
    float hum;
    float pressure;
    int mq2;
    int mq4;
    int mq135;
    bool danger;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;
DHT dht(DHT_PIN, DHT11);
Adafruit_BMP085 bmp;

// --- GLOBALS FOR TIMING & STATES ---
unsigned long lastBlink = 0;
unsigned long lastBuzzerToggle = 0;
bool ledState = LOW;
bool buzzerState = LOW;
bool bmpActive = false;

// Callback for transmission status
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print(" | Send: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize Sensors
  Wire.begin(I2C_SDA, I2C_SCL);
  dht.begin();
  if (bmp.begin()) bmpActive = true;

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;

  esp_now_register_send_cb((esp_now_send_cb_t)OnDataSent);
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
}

void loop() {
  // 1. READ ALL SENSORS
  myData.temp = dht.readTemperature();
  myData.hum = dht.readHumidity();
  myData.mq2 = analogRead(MQ2_PIN);
  myData.mq4 = analogRead(MQ4_PIN);
  myData.mq135 = analogRead(MQ135_PIN);
  myData.pressure = bmpActive ? bmp.readPressure() : -1;

  // 2. COMPREHENSIVE DANGER LOGIC
  bool gasDanger = (myData.mq2 > MQ2_LIMIT || myData.mq4 > MQ4_LIMIT || myData.mq135 > MQ135_LIMIT);
  bool tempDanger = (myData.temp > TEMP_LIMIT);
  bool humDanger = (myData.hum > HUM_HIGH || myData.hum < HUM_LOW);
  bool pressDanger = (bmpActive && myData.pressure < PRESS_MIN);

  myData.danger = (gasDanger || tempDanger || humDanger || pressDanger);

  // 3. SERIAL REPORTING (FOR DEBUGGING)
  Serial.println("-------------------------");
  Serial.printf("T:%.1f C | H:%.1f %% | P:%.0f Pa\n", myData.temp, myData.hum, myData.pressure);
  Serial.printf("MQ2:%d | MQ4:%d | MQ135:%d\n", myData.mq2, myData.mq4, myData.mq135);
  Serial.println(myData.danger ? "STATUS: !!! DANGER !!!" : "STATUS: NORMAL");

  // 4. LOCAL FEEDBACK (LEDs & BUZZER)
  if (myData.danger) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    // PULSING BUZZER: 500ms ON / 500ms OFF
    if (millis() - lastBuzzerToggle >= 50) {
      lastBuzzerToggle = millis();
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
    }
  } else {
    // SAFE STATE
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    buzzerState = LOW; 
    
    // HEARTBEAT GREEN LED
    if (millis() - lastBlink >= 1000) {
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(GREEN_LED, ledState);
    }
  }

  // 5. TRANSMIT TO BIKE UNIT
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  delay(500); // Fast loop to keep buzzer rhythm accurate
}