#define BLYNK_TEMPLATE_ID "TMPL6zm9_tUzt"
#define BLYNK_TEMPLATE_NAME "Mentoring SHT31"
#define BLYNK_AUTH_TOKEN "RL8c7s-Aph-0ZPvpmqCWl8DFWDos2my9"

#define BLYNK_PRINT Serial

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SHT31.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "ThingSpeak.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

char ssid[] = "ADNAN";
char pass[] = "15071970";

unsigned long myChannelNumber = 3088707; 
const char* myWriteAPIKey = "FQUW834C7OV1TVOH";

const int LED1 = 4;
const int LED2 = 2;

BlynkTimer timer;
WiFiClient client;

void myTimerEvent() {
  delay(2000);
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();

  if (!isnan(t) && !isnan(h)) {
    // Kirim data ke Blynk
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);

    Serial.print(F("Kelembapan: "));
    Serial.print(h);
    Serial.print(F("%  Suhu: "));
    Serial.print(t);
    Serial.println(F("°C"));

    // Kontrol LED indikator suhu >= 35°C
    if (t >= 35) {
      Blynk.virtualWrite(V2, HIGH);
      digitalWrite(LED1, HIGH);
    } else {
      Blynk.virtualWrite(V2, LOW);
      digitalWrite(LED1, LOW);
    }

    // Kirim data ke ThingSpeak
    ThingSpeak.setField(1, t);
    ThingSpeak.setField(2, h);

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Update channel ThingSpeak berhasil.");
      digitalWrite(LED2, HIGH);
    } else {
      Serial.println("Gagal update channel ThingSpeak, kode HTTP: " + String(x));
      digitalWrite(LED2, LOW);
    }
  } else {
    Serial.println("Gagal membaca sensor SHT31!");
  }
}

void setup() {
  pinMode(LED1, OUTPUT);
   pinMode(LED2, OUTPUT);
  Serial.begin(115200);

  Wire.begin();

  if (!sht31.begin(0x44)) {   // Alamat I2C default SHT31 biasanya 0x44
    Serial.println("Gagal ditemukan sensor SHT31");
    while (1) delay(1);
  }

  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Menghubungkan ke WiFi...");
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  Serial.println("\nTerhubung ke WiFi.");

  ThingSpeak.begin(client);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(3000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();
}
