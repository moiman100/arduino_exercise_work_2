#include <Wire.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(2,3,5,6,7,8);

const char password[] = {'1','2','3','4'};
char pass_mem[] = {'x','x','x','x'};

bool checkPass() {

  for (int i = 0; i < 4; i++) {
    if (password[i] != pass_mem[i]) {
      Serial.println("break");
      return false;
    }
    if (i == 3) {
      Serial.println(pass_mem);
      return true;
    }
  }
  return false;
}

void printNumbers() {
  for (int i = 0; i < 4; i++) {
          if (pass_mem[i] == 'x') {
            break;
          } else {
            lcd.print(pass_mem[i]);
          }
        }
}

/* If the master sends data to slave */
void receiveEvent(int howMany) {
  char x = Wire.read();
  if (x == 'D') {
    bool correct_pass = false;
    correct_pass = checkPass();
    for (int i = 0; i < 4; i++) {
      pass_mem[i] = 'x';
    }
    
    if (correct_pass) {
      lcd.clear();
      lcd.print("Password is correct");
    } else {
      lcd.clear();
      lcd.print("Wrong password");
    }
  } else if (x == 'C') {
    for (int i = 3; i >= 0; i--) {
      if (pass_mem[i] != 'x') {
        pass_mem[i] = 'x';
        lcd.clear();
        printNumbers();
        break;
      }
    }
  } else {
    for (int i = 0; i < 4; i++) {
      if (pass_mem[i] == 'x') {
        pass_mem[i] = x;
        lcd.clear();
        printNumbers();
        break;
      }
    }
  }
}

void setup() {

    Serial.begin(115200);
    Wire.begin(5);
    Wire.onReceive(receiveEvent);

    lcd.begin(16,2);
    lcd.setCursor(0,0);
    lcd.clear();
     
}

void loop() {
  // put your main code here, to run repeatedly:

}
