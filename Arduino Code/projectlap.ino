#define BLYNK_TEMPLATE_ID "TMPL2r0-gXqCr"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation"
#define BLYNK_AUTH_TOKEN "UIxZkzZvtrCY9_zYvEwPkkAjyc0-Rm5R"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#define FIREBASE_RTDB_ONLY
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// --- الإعدادات ---
char ssid[] = "00";
char pass[] = "00000000";
#define DATABASE_URL "https:".............." 
#define DATABASE_SECRET ".................." 

#define PUMP_RELAY_PIN 27
#define SOIL_SENSOR_PIN 34
#define LDR_SENSOR_PIN 35
#define DHT_SENSOR_PIN 14
#define DHTTYPE DHT11

DHT dht(DHT_SENSOR_PIN, DHTTYPE);
BlynkTimer timer;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
int manualSwitch = 0;

BLYNK_WRITE(V1) {
  manualSwitch = param.asInt();
}

// 1. دالة التحديث اللحظي لـ Blynk (كل ثانية)
void updateBlynkLive() {
  int soilRaw = analogRead(SOIL_SENSOR_PIN);
  int lightRaw = analogRead(LDR_SENSOR_PIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // تحديث بلينك فوراً
  Blynk.virtualWrite(V2, soilRaw);
  Blynk.virtualWrite(V3, lightRaw);
  if (!isnan(h) && !isnan(t)) {
    Blynk.virtualWrite(V4, (int)t);
    Blynk.virtualWrite(V5, (int)h);
  }

  // منطق التحكم في المضخة (يعمل كل ثانية لاستجابة سريعة)
  if ((h > 60 && t < 25.0 && lightRaw > 300) || manualSwitch == 1) {
    digitalWrite(PUMP_RELAY_PIN, HIGH);
    Blynk.virtualWrite(V6, 1);
  } else {
    digitalWrite(PUMP_RELAY_PIN, LOW);
    Blynk.virtualWrite(V6, 0);
  }
}

// 2. دالة التخزين التاريخي لـ Firebase (كل 5 دقائق)
void pushToFirebaseLog() {
  int soilRaw = analogRead(SOIL_SENSOR_PIN);
  int lightRaw = analogRead(LDR_SENSOR_PIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (Firebase.ready() && !isnan(h) && !isnan(t)) {
    FirebaseJson json;
    json.add("soil", soilRaw);
    json.add("temp", (int)t);
    json.add("hum", (int)h);
    json.add("light", lightRaw);
    json.set("timestamp/.sv", "timestamp");

    if (Firebase.RTDB.pushJSON(&fbdo, "/sensor_logs", &json)) {
      Serial.println(">>> AI Training Log Saved to Firebase");
    }
  }
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, LOW);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // إعداد تايمر منفصل لكل مهمة
  timer.setInterval(1000L, updateBlynkLive);    // تحديث بلينك كل ثانية
  timer.setInterval(300000L, pushToFirebaseLog); // تخزين فايربيس كل 5 دقائق
}

void loop() {
  Blynk.run();
  timer.run();
}