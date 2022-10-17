#include <EEPROM.h>
#include <EncoderButton.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,16,2);
EncoderButton eb1(3, 2);

const int directionBackPin = 5;
const int directionForwardPin = 6;

const int stepPin = 9;
const int dirPin = 8;

int stepperDelay;

int rate = 0;
int storedRate = EEPROM.read(0);

void onEb1Encoder(EncoderButton& eb) {
  lcd.setCursor(5,0);
  lcd.print("    ");
  lcd.setCursor(5,0);

  rate = storedRate+eb.position();
  if (rate < 0) {
    rate = 0;
  }

  if (rate > 255) {
    rate = 255;
  }
  
  EEPROM.write(0,rate);
  lcd.print(rate);
}

void setup() {
  startupLcd();
  
  pinMode(5, INPUT);
  pinMode(6, INPUT);

  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  eb1.setEncoderHandler(onEb1Encoder);

  lcd.setCursor(5,0);
  lcd.print("    ");
  lcd.setCursor(5,0);
  
  if (storedRate != 0) {
    rate = storedRate;
    lcd.print(rate);
  } else {
    rate = 0;
    lcd.print(rate);
  }
}

void loop() {
  controlDirection();
  eb1.update();
  driveStepper();
}

void controlDirection() {
  if(digitalRead(directionBackPin) == HIGH) {
    digitalWrite(dirPin,HIGH);
    lcd.setCursor(10,1);
    lcd.print("Fwd");
  }
  
  if(digitalRead(directionForwardPin) == HIGH) {
    digitalWrite(dirPin,LOW);
    lcd.setCursor(10,1);
    lcd.print("Rev");
  }
}

void startupLcd() {
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  lcd.print("Rate:");
  lcd.setCursor(0,1);
  lcd.print("Direction:");
}

void driveStepper() {
  stepperDelay = map(rate, 0, 254, 1, 9600);

  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepperDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepperDelay);
  if (stepperDelay > 300) {
    stepperDelay -= 5;
  }
}