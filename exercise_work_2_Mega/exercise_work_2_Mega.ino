#include <Wire.h>
#include <Keypad.h>

const byte num_rows = 4;
const byte num_cols = 4;
char keymap[num_rows][num_cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte row_pins[num_rows] = {2,3,4,5};
byte col_pins[num_cols] = {6,7,8,9};

Keypad keypad = Keypad(makeKeymap(keymap), row_pins, col_pins, num_rows, num_cols);

byte g_scan_error;

int motion_pin = 10;
int buzzer_pin = 11;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  while(!Serial)
  {
        
  }
  Serial.println("Scanning I2C...");
  pinMode(motion_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
}

void loop() {
  int motion_read = digitalRead(motion_pin);
  Serial.println(motion_read);

  
  char keypressed = keypad.getKey();
  if (keypressed != NO_KEY) {
    Wire.beginTransmission(5);
    Wire.write(keypressed);
    g_scan_error = Wire.endTransmission();
  }
  
  digitalWrite(buzzer_pin, motion_read);
}
