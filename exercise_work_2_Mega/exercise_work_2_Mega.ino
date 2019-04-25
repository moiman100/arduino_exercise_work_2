#include <Wire.h>
#include <Keypad.h>

#define TIMEOUT_TIME 10000
#define STATUS_MESSAGE_DELAY 2000

const byte num_rows = 4;
const byte num_cols = 4;
char keymap[num_rows][num_cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte row_pins[num_rows] = {2, 3, 4, 5};
byte col_pins[num_cols] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keymap), row_pins, col_pins, num_rows, num_cols);

byte scan_error;
byte wire_address = 1;

int motion_pin = 10;
int buzzer_pin = 11;

String password = "1234";
String pass_mem = "";

bool armed = true;
bool motion_detected = false;

unsigned long timeout_timer = 0;
bool timeout = false;

unsigned long last_message_time = 0;
unsigned long status_message_sent = 0;

void addressScan() {
  int8_t address_max = 127;

  for (byte scan_address = 1; scan_address < address_max; scan_address ++) {
    Wire.beginTransmission(scan_address);
    scan_error = Wire.endTransmission();
    if (scan_error == 0) {
      wire_address = scan_address;
      Wire.beginTransmission(wire_address);
      Wire.write("Connected");
      scan_error = Wire.endTransmission();
      last_message_time = millis();
      break;
    }
  }
}

bool checkPass() {
  if (password == pass_mem) {
    return true;
  } else {
    return false;
  }
}

void printPass() {
  char message[16];
  pass_mem.toCharArray(message, 16);
  Wire.beginTransmission(wire_address);
  Wire.write(message);
  scan_error = Wire.endTransmission();
  last_message_time = millis();
}

void clearPass() {
  pass_mem = "";
}

void backspacePass() {
  pass_mem.remove(pass_mem.length() - 1);
  printPass();
}

void writePass(char key) {
  if (pass_mem.length() < password.length()) {
    pass_mem += key;
    printPass();
  }
}

void setup() {
  Wire.begin();
  Serial.begin(115200);

  pinMode(motion_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
}

void loop() {
  if (scan_error != 0) {
    addressScan();
  }

  if (armed) {
    motion_detected = digitalRead(motion_pin) || motion_detected;
  }

  if ((millis() - timeout_timer > TIMEOUT_TIME) && armed && !timeout) {
    timeout = true;
    Wire.beginTransmission(wire_address);
    Wire.write("Timeout");
    scan_error = Wire.endTransmission();
  }

  if (!timeout) {
    if ((millis() - last_message_time > STATUS_MESSAGE_DELAY) && pass_mem == "" && status_message_sent <= last_message_time) {
      Wire.beginTransmission(wire_address);
      if (armed) {
        Wire.write("Armed");
      } else {
        Wire.write("Disarmed");
      }
      scan_error = Wire.endTransmission();
      status_message_sent = millis();
    }

    char keypressed = keypad.getKey();

    if (!armed && keypressed != 'A') {
      keypressed = NO_KEY;
    }

    switch (keypressed) {
      case NO_KEY :
        break;
      case 'D' :
        Wire.beginTransmission(wire_address);
        if (checkPass()) {
          armed = false;
          motion_detected = false;
          Wire.write("Right password");
        } else {
          Wire.write("Wrong password");
        }
        scan_error = Wire.endTransmission();
        last_message_time = millis();

        clearPass();
        break;
      case 'C' :
        backspacePass();
        break;
      case 'A' :
        armed = true;
        status_message_sent = 0;
        last_message_time = 0;
        timeout_timer = millis();
        break;
      default :
        writePass(keypressed);
        break;
    }
  }

  if (armed && motion_detected) {
    digitalWrite(buzzer_pin, HIGH);
  } else {
    digitalWrite(buzzer_pin, LOW);
    timeout_timer = millis();
  }
}
