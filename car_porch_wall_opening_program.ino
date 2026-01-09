#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9

#define SERVO1_PIN 3   // +90°
#define SERVO2_PIN 5   // -90°

MFRC522 mfrc522(SS_PIN, RST_PIN);

Servo servo1;
Servo servo2;

// Authorized UIDs
byte tagUID[4]  = {0xE7, 0x51, 0x32, 0x06};
byte cardUID[4] = {0x91, 0x30, 0x1B, 0x06};

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  servo1.write(0);     // home position
  servo2.write(180);   // opposite direction home

  Serial.println("Scan RFID Tag or Card...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Scanned UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  if (checkUID(tagUID) || checkUID(cardUID)) {
    Serial.println("Authorized!");

    servo1.write(90);   // +90°
    servo2.write(90);   // -90° (opposite side)

    delay(5000);

    servo1.write(0);
    servo2.write(180);

  } else {
    Serial.println("Unauthorized UID");
  }

  Serial.println("--------------------------------");

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

bool checkUID(byte *allowedUID) {
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != allowedUID[i]) {
      return false;
    }
  }
  return true;
}
