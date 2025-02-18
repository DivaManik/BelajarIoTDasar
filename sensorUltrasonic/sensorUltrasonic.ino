// Define Variable
const int trigPin = 15;
const int echoPin = 12;

long duration;
int distance;

void setup() {
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  Serial.begin(9600);
}

void loop(){
  // Clears the trigPin
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
  delay(200);
}