#define BLYNK_TEMPLATE_ID "TMPL6XgJyztZ8"
#define BLYNK_TEMPLATE_NAME "Diva"
#define BLYNK_AUTH_TOKEN "ADY6-28YoezMFJuVvuBnB4H7prA4766S"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// **Pin Konfigurasi**
#define DHTPIN 14  // Gunakan GPIO14 (D5) agar tidak berbenturan dengan relay
#define DHTTYPE DHT11  

int relay_pin = 5;  // GPIO5 (D1 pada NodeMCU)
bool manualControl = false;  // Status kontrol manual relay

int lampuDingin = 16;  // GPIO16 (D0)
int lampuNormal = 4;   // GPIO4 (D2)
int lampuPanas = 15;   // GPIO15 (D8)

DHT dht(DHTPIN, DHTTYPE);
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "sixbrothers";
char pass[] = "abcpassword@123";

// Timer untuk membaca suhu tanpa delay
unsigned long previousMillis = 0;
const long interval = 2000; // Baca DHT setiap 2 detik

// Fungsi untuk membaca status tombol virtual (V0) di Blynk
BLYNK_WRITE(V0) {
  int state = param.asInt();  // Baca status tombol (1 = ON, 0 = OFF)
  manualControl = state;
  digitalWrite(relay_pin, state ? LOW : HIGH);  // LOW = ON, HIGH = OFF
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  
  pinMode(relay_pin, OUTPUT);
  pinMode(lampuDingin, OUTPUT);
  pinMode(lampuNormal, OUTPUT);
  pinMode(lampuPanas, OUTPUT);

  digitalWrite(relay_pin, HIGH);  // Relay OFF saat awal (karena active low)
  digitalWrite(lampuDingin, LOW);
  digitalWrite(lampuNormal, LOW);
  digitalWrite(lampuPanas, LOW);
  
  dht.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    bacaDHT();
  }

  Blynk.run();
}

void bacaDHT() {
  float suhu = dht.readTemperature();  
  float kelembaban = dht.readHumidity();
  float hic = dht.computeHeatIndex(suhu, kelembaban,false);

  if (isnan(suhu) || isnan(kelembaban)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Kirim data ke Blynk
  Blynk.virtualWrite(V1, suhu);
  Blynk.virtualWrite(V2, kelembaban);
  Blynk.virtualWrite(V3, hic);

  // **Kontrol relay dan switch di Blynk**
  if (!manualControl) {
    if (suhu > 36) {
      digitalWrite(relay_pin, LOW);  // Relay ON (Dinamo Menyala)
      Blynk.virtualWrite(V0, 1);     // Switch di Blynk otomatis ke ON
    } else {
      digitalWrite(relay_pin, HIGH);  // Relay OFF (Dinamo Mati)
      Blynk.virtualWrite(V0, 0);     // Switch di Blynk otomatis ke OFF
    }
  }

  // **Kontrol lampu indikator di Breadboard dan Blynk**
  if (suhu < 27) {
    digitalWrite(lampuDingin, HIGH);
    digitalWrite(lampuNormal, LOW);
    digitalWrite(lampuPanas, LOW);
    
    Blynk.virtualWrite(V4, HIGH);
    Blynk.virtualWrite(V5, LOW);
    Blynk.virtualWrite(V6, LOW);
  } 
  else if (suhu >= 27 && suhu <= 36) {
    digitalWrite(lampuDingin, LOW);
    digitalWrite(lampuNormal, HIGH);
    digitalWrite(lampuPanas, LOW);
    
    Blynk.virtualWrite(V4, LOW);
    Blynk.virtualWrite(V5, HIGH);
    Blynk.virtualWrite(V6, LOW);
  } 
  else {  // suhu > 36
    digitalWrite(lampuDingin, LOW);
    digitalWrite(lampuNormal, LOW);
    digitalWrite(lampuPanas, HIGH);
    
    Blynk.virtualWrite(V4, LOW);
    Blynk.virtualWrite(V5, LOW);
    Blynk.virtualWrite(V6, HIGH);
  }

  // Debugging ke Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(suhu);
  Serial.println(" °C");
}
