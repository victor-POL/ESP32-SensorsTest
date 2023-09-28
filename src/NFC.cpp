#include <MFRC522.h>

#define SS_PIN 5   // ESP32 pin GPIO5
#define RST_PIN 15 // ESP32 pin GPIO27

extern String event;

class NFC
{
private:
    MFRC522 rfid;

    byte keychainID[4] = {26, 164, 251, 176};
    byte cardID[4] = {195, 226, 203, 169};

public:
    NFC()
    {
        this->rfid = MFRC522(SS_PIN, RST_PIN);
    }

    void Setup()
    {
        SPI.begin();
        this->rfid.PCD_Init();
    }

    bool CheckStatus()
    {
        if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        {
            return false;
        }

        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
        if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
        {
            Serial.println("// No es nfc valido");
            event = "Event::NFCIncorrecto";
        }
        else if (memcmp(rfid.uid.uidByte, cardID, 4) == 0)
        {
            Serial.println("// Tarjeta leida");
            event = "Event::AperturaNFC";
        }
        else if(memcmp(rfid.uid.uidByte, keychainID, 4) == 0)
        {
            Serial.println("// Llavero leido");
            event = "Event::NFCIncorrecto";
        }
        // Halt PICC
        Serial.println("// Halt PICC");
        rfid.PICC_HaltA();
        // Stop encryption on PCD
        Serial.println("// Stop encryption on PCD");
        rfid.PCD_StopCrypto1();
        Serial.println("-----------------------");
        return true;
    }
};