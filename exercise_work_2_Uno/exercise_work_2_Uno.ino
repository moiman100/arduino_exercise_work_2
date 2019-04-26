// Group 5

#include <Wire.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 5, 6, 7, 8);


// Receive data from master and show it on LCD
void receiveEvent(int howMany) {
  String received_message = "";
  char received_char;
  
  while (Wire.available()) {
    received_char = Wire.read();
    received_message += received_char;
  }
  
  lcd.clear();
  lcd.print(received_message);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(5);
  Wire.onReceive(receiveEvent);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("No connection");
}

void loop() {
  delay(250);
}
