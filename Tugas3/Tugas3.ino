#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int trigPin = 16;
const int echoPin = 13;
const int potPin = A0;
const int led1Pin = 14;
const int led2Pin = 12;

const char *ssid = "Wifi Pribadi";
const char *password = "Inipassword";
const char *mqtt_broker = "broker.emqx.io";
const int mqtt_port = 1883;
const char *mqtt_username = "diva";
const char *mqtt_password = "";

const char *topicDekat = "71220863/Dekat";
const char *topicSedang = "71220863/Sedang";
const char *topicJauh = "71220863/Jauh";
const char *topicSangatJauh = "71220863/Sangat_Jauh";
// const char *topicPWM = "pwm/71220863";
// const char *topicVoltage = "voltage/71220863";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastUpdate = 0;
bool showDistance = true;

void setup() {
  Serial.begin(115200);
  
  lcd.begin();
  lcd.backlight();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Menghubungkan ke WiFi...");
  }
  Serial.println("WiFi Terhubung!");

  client.setServer(mqtt_broker, mqtt_port);
  while (!client.connected()) {
    String client_id = "esp8266-client-" + String(WiFi.macAddress());
    Serial.printf("Menyambungkan ke MQTT sebagai %s\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Terhubung ke MQTT Broker!");
    } else {
      Serial.print("Gagal, status: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void loop() {
  // === Membaca Jarak Sensor Ultrasonik ===
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 20000);
  if (duration == 0) {
    Serial.println("Sensor tidak merespon!");
    delay(500);
    return;
  }

  float distance = duration * 0.0343 / 2;
  String kondisi;
  const char *mqttTopic;

  if (distance < 8) {
    kondisi = "Dekat";
    mqttTopic = topicDekat;
  } else if (distance >= 8 && distance <= 15) {
    kondisi = "Sedang";
    mqttTopic = topicSedang;
  } else if (distance > 15 && distance <= 30) {
    kondisi = "Jauh";
    mqttTopic = topicJauh;
  } else {
    kondisi = "Sangat Jauh";
    mqttTopic = topicSangatJauh;
  }

  // === Membaca Nilai Potensiometer ===
  int potValue = analogRead(potPin);
  // int pwmValue = map(potValue, 0, 1023, 0, 255);
  int pwmValue = constrain(map(potValue, 0, 1023, 0, 255), 0, 255);
  float voltage = potValue * (5.0 / 1023.0);

  analogWrite(led1Pin, pwmValue);
  analogWrite(led2Pin, pwmValue);

  // === Kirim Data ke MQTT ===
  String mqttMessage = "Jarak: " + String(distance, 1) + " cm\nKondisi : "+ kondisi;
  client.publish(mqttTopic, mqttMessage.c_str());
  delay(500);


  // === Update LCD setiap 1 detik ===
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    lcd.clear();

    if (showDistance) {
      lcd.setCursor(1, 0);
      lcd.print("Jarak: ");
      lcd.print(distance, 1);
      lcd.print("cm");

      lcd.setCursor(1, 1);
      lcd.print("Kondisi: ");
      lcd.print(kondisi);
    } else {
      lcd.setCursor(1, 0);
      lcd.print("Nilai PWM: ");
      lcd.print(pwmValue);

      lcd.setCursor(1, 1);
      lcd.print("Voltage: ");
      lcd.print(voltage, 2);
    }
    showDistance = !showDistance;
  }

  Serial.printf("Jarak: %.1f cm | Kondisi: %s | PWM: %d | Tegangan: %.2fV\n",
                distance, kondisi.c_str(), pwmValue, voltage);
  delay(500);

  client.loop();
}
