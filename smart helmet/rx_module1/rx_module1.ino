#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// --- PIN CONFIGURATION ---
#define GREEN_LED 14 
#define RED_LED 27
#define BUZZER_PIN 26

// --- LCD CONFIGURATION ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- DATA STRUCTURE (Must match Transmitter exactly) ---
typedef struct struct_message {
    float temp;
    float hum;
    float pressure;
    int mq2;
    int mq6;
    int mq135;
    bool danger;
} struct_message;

struct_message myData;

// --- GLOBALS FOR TIMING & STATES ---
unsigned long lastBlink = 0;
unsigned long lastBuzzerToggle = 0;
unsigned long lastRecvTime = 0; // To track if we lost connection
bool ledState = LOW;
bool buzzerState = LOW;

// --- CALLBACK: Triggered when data is received ---
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  lastRecvTime = millis(); // Update "Last Seen" timestamp

  // 1. PRINT TO SERIAL MONITOR
  Serial.println("--- RECEIVED FROM HELMET ---");
  Serial.printf("T:%.1fC | H:%.1f%% | P:%.0fPa\n", myData.temp, myData.hum, myData.pressure);
  Serial.printf("MQ2:%d | MQ6:%d | MQ135:%d\n", myData.mq2, myData.mq6, myData.mq135);
  Serial.println(myData.danger ? "ALARM: ACTIVE" : "ALARM: SAFE");

  // 2. UPDATE LCD DISPLAY
  lcd.clear();
  if (myData.danger) {
    lcd.setCursor(0, 0);
    lcd.print("!!! DANGER !!!");
    lcd.setCursor(0, 1);
    lcd.print("P:"); lcd.print(myData.pressure, 1);
    lcd.print(" G:"); lcd.print(myData.mq135);
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Temp:"); lcd.print(myData.temp, 1);
    lcd.print("CHum:"); lcd.print(myData.hum, 0);
    lcd.setCursor(0, 1);
    lcd.print("System Status:OK");
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Waiting for");
  lcd.setCursor(0,1);
  lcd.print("Helmet Link...");

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register the receive callback
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Check if we have received data recently (Connection Guard)
  bool isConnected = (millis() - lastRecvTime < 3000);

  if (isConnected) {
    if (myData.danger) {
      // --- DANGER STATE (Mirroring Transmitter) ---
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);

      // PULSING BUZZER (500ms ON / 500ms OFF)
      if (millis() - lastBuzzerToggle >= 500) {
        lastBuzzerToggle = millis();
        buzzerState = !buzzerState;
        digitalWrite(BUZZER_PIN, buzzerState);
      }
    } else {
      // --- SAFE STATE ---
      digitalWrite(RED_LED, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      buzzerState = LOW;

      // HEARTBEAT GREEN LED (Indicates data is flowing)
      if (millis() - lastBlink >= 1000) {
        lastBlink = millis();
        ledState = !ledState;
        digitalWrite(GREEN_LED, ledState);
      }
    }
  } else {
    // --- NO CONNECTION STATE ---
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    
    // Show signal loss on LCD
    static unsigned long lastLcdUpdate = 0;
    if (millis() - lastLcdUpdate > 2000) {
        lcd.clear();
        lcd.print("Searching...");
        lastLcdUpdate = millis();
    }
  }
}