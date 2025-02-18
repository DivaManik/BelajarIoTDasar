int button = 5;
int led = 16;
int temp = 0;  // Menyimpan status LED (0 = mati, 1 = nyala)
int lastButtonState = HIGH; // Menyimpan status terakhir tombol

void setup() {
  pinMode(button, INPUT_PULLUP); // Menggunakan internal pull-up
  pinMode(led, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int buttonState = digitalRead(button); // Membaca status tombol
  
  // Cek apakah tombol ditekan (LOW) dan sebelumnya belum ditekan
  if (buttonState == LOW && lastButtonState == HIGH) {
    temp = !temp; // Toggle nilai temp (0 jadi 1, 1 jadi 0)

    // Mengontrol LED dengan if-else
    if (temp == 1) {
      digitalWrite(led, HIGH);
    } else {
      digitalWrite(led, LOW);
    }

    Serial.print("Tombol ditekan, status temp: ");
    Serial.println(temp);
    delay(200); // Debounce delay untuk mencegah bouncing
  }
  
  lastButtonState = buttonState; // Simpan status tombol terakhir
}
