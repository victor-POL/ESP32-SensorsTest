#include <Arduino.h>
#include <ESP32Servo.h>
#include <Keypad.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include <MFRC522.h>

// Buzzer
#define STATUS_KEY_SOUND 10
#define STATUS_SUCESS_SOUND 11
#define STATUS_FAIL_SOUND 12
#define STATUS_NO_SOUND 13

#define BUZZER_PIN 2

#define BUZZER_CHANNEL 4
#define BUZZER_RESOLUTION 8
#define BUZZER_FREQUENCY 2000

#define BUZZER_SUCCESS_FREQ 800
#define BUZZER_FAIL_FREQ 200
#define BUZZER_KEY_FREQ 500

#define BUZZER_KEY_DURATION 100
#define BUZZER_SUCCESS_DURATION 3000
#define BUZZER_FAIL_DURATION 3000

int timeSoundActivated = 0;
int lastTimeBuzzer = 0;
int status;
bool buzzerOn = false;

// Servo
#define SERVO_PIN 26

Servo servo = Servo();

// Relay
#define RELAY_PIN 25

// Photoresistor
#define PHOTORESISTOR_PIN 39

const float GAMMA = 0.7;
const float RL10 = 50;

// Keypad
#define ROWS_KEYPAD 4
#define COLS_KEYPAD 4
#define KEYS_ROW_1 '1', '2', '3', 'A'
#define KEYS_ROW_2 '4', '5', '6', 'B'
#define KEYS_ROW_3 '7', '8', '9', 'C'
#define KEYS_ROW_4 '*', '0', '#', 'D'
#define KEYPAD_ROW_1_PIN 13
#define KEYPAD_ROW_2_PIN 12
#define KEYPAD_ROW_3_PIN 14
#define KEYPAD_ROW_4_PIN 27
#define KEYPAD_COLUMN_1_PIN 17
#define KEYPAD_COLUMN_2_PIN 16
#define KEYPAD_COLUMN_3_PIN 4
#define KEYPAD_COLUMN_4_PIN 15

char keys[ROWS_KEYPAD][COLS_KEYPAD] = {
    {KEYS_ROW_1},
    {KEYS_ROW_2},
    {KEYS_ROW_3},
    {KEYS_ROW_4}};

uint8_t rowPins[ROWS_KEYPAD] = {
    KEYPAD_ROW_1_PIN,
    KEYPAD_ROW_2_PIN,
    KEYPAD_ROW_3_PIN,
    KEYPAD_ROW_4_PIN,
};
uint8_t columnPins[COLS_KEYPAD] = {
    KEYPAD_COLUMN_1_PIN,
    KEYPAD_COLUMN_2_PIN,
    KEYPAD_COLUMN_3_PIN,
    KEYPAD_COLUMN_4_PIN,
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, ROWS_KEYPAD, COLS_KEYPAD);

// Ultrasonic entrance
#define ENTRANCE_SENSOR_TRIGGER_PIN 32
#define ENTRANCE_SENSOR_ECHO_PIN 34

// Ultrasonic door
#define DOOR_SENSOR_TRIGGER_PIN 33
#define DOOR_SENSOR_ECHO_PIN 35

// LCD
rgb_lcd lcd;

// NFC
#define SS_PIN  5  // ESP32 pin GPIO5 
#define RST_PIN 27 // ESP32 pin GPIO27 

MFRC522 rfid(SS_PIN, RST_PIN);

void setup()
{
    Serial.begin(115200);

    // setupBuzzer();

    // setupServo();

    // setupRelay();

    // setupPhotoresistor();

    // setupKeypad();

    // setupUltrasonicEntrance();

    // setupUltrasonicDoor();

    // setupLCD();

    setupNFC();
}

void loop()
{
    testNFC();
}

// Buzzer

void setupBuzzer()
{
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQUENCY, BUZZER_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
}

void testBuzzerKeySoundAndDuration()
{
    ledcWriteTone(BUZZER_CHANNEL, BUZZER_KEY_FREQ);
    delay(BUZZER_KEY_DURATION);
    ledcWriteTone(BUZZER_CHANNEL, 0);
}

void testBuzzerSuccesSoundAndDuration()
{
    ledcWriteTone(BUZZER_CHANNEL, BUZZER_SUCCESS_FREQ);
    delay(BUZZER_SUCCESS_DURATION);
    ledcWriteTone(BUZZER_CHANNEL, 0);
}

void testBuzzerFailSoundAndDuration()
{
    ledcWriteTone(BUZZER_CHANNEL, BUZZER_FAIL_FREQ);
    delay(BUZZER_FAIL_DURATION);
    ledcWriteTone(BUZZER_CHANNEL, 0);
}

bool reachedTimeout()
{
    int currentTime = millis();
    int timeElapsed = currentTime - timeSoundActivated;
    int limit = 0;

    switch (status)
    {
    case STATUS_KEY_SOUND:
        limit = BUZZER_KEY_DURATION;
        break;
    case STATUS_SUCESS_SOUND:
        limit = BUZZER_SUCCESS_DURATION;
        break;
    case STATUS_FAIL_SOUND:
        limit = BUZZER_FAIL_DURATION;
        break;
    }

    return timeElapsed > limit;
}

void testBuzzer()
{
    testBuzzerKeySoundAndDuration();
    delay(2000);
    testBuzzerSuccesSoundAndDuration();
    delay(2000);
    testBuzzerFailSoundAndDuration();
    delay(2000);
}

void setupServo()
{
    servo.attach(SERVO_PIN, 500, 2500);
    servo.write(0);
}

void testServo()
{
    delay(3000);
    servo.write(179);
    delay(3000);
    servo.write(0);
    delay(3000);
}

// Relay

void setupRelay()
{
    pinMode(RELAY_PIN, OUTPUT);
}

void testRelay()
{
    digitalWrite(RELAY_PIN, HIGH);
    delay(2500);
    digitalWrite(RELAY_PIN, LOW);
    delay(2500);
}

// Photoresistor

void setupPhotoresistor()
{
    pinMode(PHOTORESISTOR_PIN, INPUT);
}

void testPhotoresistor()
{
    int analogValue = analogRead(PHOTORESISTOR_PIN);
    float voltage = analogValue / 1024. * 5;
    float resistance = 2000 * voltage / (1 - voltage / 5);
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

    Serial.println(analogValue);

    delay(2000);
}

// Keypad

void setupKeypad()
{
    pinMode(KEYPAD_ROW_1_PIN, INPUT);
    pinMode(KEYPAD_ROW_2_PIN, INPUT);
    pinMode(KEYPAD_ROW_3_PIN, INPUT);
    pinMode(KEYPAD_ROW_4_PIN, INPUT);
    pinMode(KEYPAD_COLUMN_1_PIN, INPUT);
    pinMode(KEYPAD_COLUMN_2_PIN, INPUT);
    pinMode(KEYPAD_COLUMN_3_PIN, INPUT);
    pinMode(KEYPAD_COLUMN_4_PIN, INPUT);
}

void testKeypad()
{
    char key = keypad.getKey();
    if (key)
    {
        Serial.println(key);
    }
}

void testKeypadWithBuzzer()
{
    if (status != STATUS_NO_SOUND && reachedTimeout() == true)
    {
        ledcWriteTone(BUZZER_CHANNEL, 0);
        status = STATUS_NO_SOUND;
    }

    char key = keypad.getKey();
    if (key)
    {
        Serial.println(key);

        switch (key)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
            buzzerOn = true;
            timeSoundActivated = millis();
            status = STATUS_KEY_SOUND;
            ledcWriteTone(BUZZER_CHANNEL, BUZZER_KEY_FREQ);
            break;
        case '*':
            buzzerOn = true;
            timeSoundActivated = millis();
            status = STATUS_SUCESS_SOUND;
            ledcWriteTone(BUZZER_CHANNEL, BUZZER_SUCCESS_FREQ);

            break;
        case '#':
            buzzerOn = true;
            timeSoundActivated = millis();
            status = STATUS_FAIL_SOUND;
            ledcWriteTone(BUZZER_CHANNEL, BUZZER_FAIL_FREQ);
            break;
        }
    }
}

// Ultrasonic entrance
void setupUltrasonicEntrance()
{
    pinMode(ENTRANCE_SENSOR_TRIGGER_PIN, OUTPUT);
    pinMode(ENTRANCE_SENSOR_ECHO_PIN, INPUT);
}

int getDistanceEntrance()
{
    digitalWrite(ENTRANCE_SENSOR_TRIGGER_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(ENTRANCE_SENSOR_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(ENTRANCE_SENSOR_TRIGGER_PIN, LOW);

    return 0.01723 * pulseIn(ENTRANCE_SENSOR_ECHO_PIN, HIGH);
}

void testUltrasonicEntrance()
{
    int distance = getDistanceEntrance();
    Serial.print("Entrada: ");
    Serial.println(distance);
    delay(2000);
}

// Ultrasonic door
void setupUltrasonicDoor()
{
    pinMode(DOOR_SENSOR_TRIGGER_PIN, OUTPUT);
    pinMode(DOOR_SENSOR_ECHO_PIN, INPUT);
}

int getDistanceDoor()
{
    digitalWrite(DOOR_SENSOR_TRIGGER_PIN, LOW);
    delayMicroseconds(2);

    digitalWrite(DOOR_SENSOR_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);

    digitalWrite(DOOR_SENSOR_TRIGGER_PIN, LOW);

    return 0.01723 * pulseIn(DOOR_SENSOR_ECHO_PIN, HIGH);
}

void testUltrasonicDoor()
{
    int distance = getDistanceDoor();
    Serial.print("Puerta: ");
    Serial.println(distance);
    delay(2000);
}

// LCD
void setupLCD()
{
    lcd.begin(16, 2);
}

void testLCD()
{
    lcd.setRGB(0, 150, 0);
    // set the cursor to (0,0):
    lcd.setCursor(0, 0);
    // print from 0 to 9:
    for (int thisChar = 0; thisChar < 10; thisChar++) {
        lcd.print(thisChar);
        delay(500);
    }

    // set the cursor to (16,1):
    lcd.setCursor(16, 1);
    // set the display to automatically scroll:
    lcd.autoscroll();
    // print from 0 to 9:
    for (int thisChar = 0; thisChar < 10; thisChar++) {
        lcd.print(thisChar);
        delay(500);
    }
    // turn off automatic scrolling
    lcd.noAutoscroll();

    // clear screen for the next loop:
    lcd.clear();
}

// NFC
void setupNFC()
{
    SPI.begin(); // init SPI bus
    rfid.PCD_Init(); // init MFRC522

    Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
}

void testNFC()
{
    if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print UID in Serial Monitor in the hex format
      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}