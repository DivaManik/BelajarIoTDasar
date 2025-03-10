#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// WiFi Credentials
const char *ssid = "Redmi 9T"; // Nama WiFi
const char *password = "099887766"; // Password WiFi

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topicDHT = "71220863/DHT";
const char *topicPING = "71220863/PING";
const char *mqtt_username = "";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

// Pin Definitions
#define DHTPIN 16  // D0
#define TRIG_PIN 4  // D2
#define ECHO_PIN 0  // D3
#define BUTTON_PIN 14 // D5
#define LED1 12  // D6
#define LED2 13  // D7
#define LED3 15  // D8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

bool publishDHT = true;

void setup() {
    Serial.begin(115200);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    dht.begin();
    connectWiFi();
    client.setServer(mqtt_broker, mqtt_port);
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();

    if (digitalRead(BUTTON_PIN) == LOW) {
    delay(200);  // Debounce
    while (digitalRead(BUTTON_PIN) == LOW);  // Tunggu tombol dilepas
    publishDHT = !publishDHT;
    if (publishDHT) {
        float temp = dht.readTemperature();
        client.publish(topicDHT, String(temp).c_str());
        controlLEDsDHT(temp);
    } else {
        float distance = getDistance();
        client.publish(topicPING, String(distance).c_str());
        controlLEDsPING(distance);
    }
}
}

void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

void reconnectMQTT() {
    while (!client.connected()) {
        if (client.connect("ESP8266Client")) {
            Serial.println("Connected to MQTT");
        } else {
            delay(5000);
        }
    }
}

float getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    float duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.034 / 2;
}

void controlLEDsDHT(float temp) {
    digitalWrite(LED1, temp < 20);
    digitalWrite(LED2, temp >= 20 && temp < 28);
    digitalWrite(LED3, temp >= 28);
}

void controlLEDsPING(float distance) {
    digitalWrite(LED1, distance < 10);
    digitalWrite(LED2, distance >= 10 && distance < 19);
    digitalWrite(LED3, distance >= 20);
}
