#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

int cambien = A0;
int giatri;
int loa = 7;
#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 5
Servo myServo; // Khai báo đối tượng Servo
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Địa chỉ I2C của màn hình LCD và số cột, số hàng

bool isRFIDVerified = false;
bool isGasDetected = false;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight(); // Bật đèn nền
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Approach the RFID card to the reader...");
  Serial.println();
  myServo.attach(SERVO_PIN);
  pinMode(loa, OUTPUT);
  pinMode(cambien, INPUT);
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    isRFIDVerified = false;
  } else if (!mfrc522.PICC_ReadCardSerial()) {
    isRFIDVerified = false;
  } else {
    String content = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    
    if (content.substring(1) == "03 89 86 16") { // Thay đổi ID tùy thuộc vào thẻ RFID của bạn
      lcd.setCursor(3, 0);
      lcd.print("ID HOP LE");
      lcd.setCursor(2, 1);
      lcd.print("welcome home");
      myServo.write(0);
      tone(loa, 3000, 1000);
      delay(3000);
      noTone(loa);
      myServo.write(90);
      lcd.clear();
      
      isRFIDVerified = true;
    } else {
      lcd.setCursor(1, 0);
      lcd.print("ID KHONG HOP LE");
      lcd.setCursor(0, 1);
      lcd.print("set a true card ");
      tone(loa, 3000, 1000);
      delay(3000);
      noTone(loa);
      lcd.clear();
      
      isRFIDVerified = false;
    }
  }

  giatri = analogRead(cambien);
  if (giatri > 500) {
    lcd.setCursor(0, 0);
    lcd.print("Gas dangerous");
    tone(loa, 3000, 1000);
    myServo.write(0);
    delay(5000);
    noTone(loa);
    lcd.clear();
    myServo.write(90);
    
    isGasDetected = true;
  } else {
    isGasDetected = false;
  }
  
  if (isGasDetected && !isRFIDVerified) {
    lcd.setCursor(0,0);
    lcd.print("Gas level: ");
    lcd.print(giatri); // Hiển thị giá trị cảm biến
    delay(1000); // Đợi 1 giây trước khi xóa màn hình
    lcd.clear();
  }

  Serial.print("giá trị cảm biến : ");
  Serial.println(giatri);
  delay(200);
}
