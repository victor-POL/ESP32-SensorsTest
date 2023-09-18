#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#define RST_PIN 22
#define SS_PIN 21
MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup()
{
    Serial.begin(115200);

    SPI.begin();
    mfrc522.PCD_Init();
    mfrc522.PCD_DumpVersionToSerial();
    Serial.println(F(">>> Módulo RFID activado! \n"));
}

void loop()
{

    byte cardUID[4];
    String uid, estado, user;
    int pinAux;

    if (mfrc522.PICC_IsNewCardPresent())
    { // Look for new cards
        if (mfrc522.PICC_ReadCardSerial())
        { // Select one of the cards

            Serial.print(F("\n---> Tarjeta detectada con uuID:  "));
            for (byte i = 0; i < mfrc522.uid.size; i++)
            {
                cardUID[i] = mfrc522.uid.uidByte[i];
                uid += String(cardUID[i], HEX);
            }
            Serial.println(uid);
        }
    }
}