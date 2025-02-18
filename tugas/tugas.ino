// Inisiasi Variable LED
int led= 16;
// Inisiasi variable Button
int button = 5;
int temp = 0;
int prevbuttonStatus = HIGH;
// Inisiasi Variable Sensor Ultrasonik
int trigPin = 15;
int echoPin = 12;
long duration;
int distance;


#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(button,INPUT_PULLUP);
  pinMode(led,OUTPUT);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);


  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  dht.begin();
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
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      delay(100);
    }else{
      digitalWrite(led,LOW);
    }
    delay(200);
    
  }else {
    delay(1000);

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    if(t > 35){
      digitalWrite(led,HIGH);
      delay(100);
      digitalWrite(led,LOW);
      delay(100);
    }else{
      digitalWrite(led,LOW);
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
  }
  delay(200);


}
