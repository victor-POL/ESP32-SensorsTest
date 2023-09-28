#include <Arduino.h>
#include <ESP32Servo.h>
#include <Keypad.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <SPI.h>
#include <MFRC522.h>
#include "NFC.cpp"

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
#define COLS_KEYPAD 3
#define KEYS_ROW_1 '1', '2', '3'
#define KEYS_ROW_2 '4', '5', '6'
#define KEYS_ROW_3 '7', '8', '9'
#define KEYS_ROW_4 '*', '0', '#'
#define KEYPAD_ROW_1_PIN 17
#define KEYPAD_ROW_2_PIN 16
#define KEYPAD_ROW_3_PIN 4
#define KEYPAD_ROW_4_PIN 13
#define KEYPAD_COLUMN_1_PIN 12
#define KEYPAD_COLUMN_2_PIN 14
#define KEYPAD_COLUMN_3_PIN 27

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

// Button
#define BUTTON_PIN 36

int previousButtonState = LOW;

// NFC
String event = "";

NFC nfc = NFC();

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

    setupLCD();

    // nfc.Setup();

    // setupButton();
}

void loop()
{
    testLCD();
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
    // LoadInputNewPass
    // 1. ClearScreen();
    lcd.clear();

    // 2. ShowMessage("Nueva clave:", 0);
    // - SetCursor(0, line);
    lcd.setCursor(0, 0);
    // - Print(message);
    lcd.print("Nueva clave:");


    // 3. SetCursor(0, 1);
    lcd.setCursor(0, 1);

    /////////////////////////////////////////
    delay(5000);
    /////////////////////////////////////////
    // ClearNewPassEnteredOnScreen
}

// Button
void setupButton()
{
    pinMode(BUTTON_PIN, INPUT);
}

void testButton()
{
    int buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != previousButtonState)
    {
        Serial.println(buttonState);
        previousButtonState = buttonState;
    }
}