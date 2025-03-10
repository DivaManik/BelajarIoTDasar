#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
// WiFi
const char *ssid = "Redmi 9T"; // Enter your WiFi name
const char *password = "099887766";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic = "71220863/DHT";
const char *topic2 = "71220863/PING";
const char *mqtt_username = "qwerty";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

// Inisiasi Variable LED
int led1= 12;
int led2= 13;
int led3= 15;


// Inisiasi variable Button
int button = 14;
int temp = 0;
int prevbuttonStatus = HIGH;
// Inisiasi Variable Sensor Ultrasonik
int trigPin = 0;
int echoPin = 4;
long duration;
int distance;



#define DHTPIN 2     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
 
  // Set software serial baud to 115200;
  // pinMode(LED_PIN, OUTPUT);
  // digitalWrite(LED_PIN, LOW);
  // Setup Awal
  pinMode(button,INPUT_PULLUP);
  pinMode(led1,OUTPUT);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);


  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();
  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public emqx mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
      }
  }
  // publish and subscribe

  client.subscribe(topic);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {

  int buttonStatus = digitalRead(button);

  if (buttonStatus == LOW && prevbuttonStatus == HIGH){
    if (temp == 0){
      temp = 1;
    }else {
      temp =0;
    }
  }

  if (temp == 1){
    digitalWrite(trigPin,LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin,HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin,LOW);
    duration = pulseIn(echoPin,HIGH);
    distance = duration * 0.043/2;
    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" cm");
    
    if (distance < 10 ){
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
    }else if (distance > 10 && distance <19){
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
    } else {
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
    }
    delay(200);
    
    String msg = "Distance : "+ String(distance) +" cm" ;
    client.publish(topic2, msg.c_str());
    
  }else {
    delay(1000);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);


    String msg = "Temperatur : "+ String(t) +" C" ;
    client.publish(topic, msg.c_str());
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    if(t < 20){
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
    }else if(t >= 20 && t < 28){
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
    }else{
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    // Serial.print(F("Humidity: "));
    // Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    // Serial.print(F("°F  Heat index: "));
    // Serial.print(hic);
    // Serial.print(F("°C "));
    // Serial.print(hif);
    // Serial.println(F("°F"));
  }
  delay(200);

  client.loop();
  
}