// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"
DHT dht(1. vec3, DHT);
int sensorSuhu =13; // Sensor Suhu
// Variable Sensor jarak
int echoPin = 4;
int trigPin = 5;  
// Variable Button
int inputButton = 16;
// Varianle LED
int ledInput = 12;



void setup() {
  // Setup Sensor jarak
    Serial.begin(9600);
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);

  //Setup Button
  pinMode(inputButton,INPUT_PULLUP);
  // Setup LED
  pinMode(ledInput,OUTPUT);



  Serial.println(F("DHTxx test!"));

  dht.begin();
}

void loop() {
  Serial.println("H");

  digitalWrite(ledInput,HIGH);

  
}
