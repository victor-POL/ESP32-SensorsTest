#include <Arduino.h>
#include <ESP32Servo.h>
#include <Keypad.h>

// Buzzer
#define BUZZER_PIN 27

#define BUZZER_CHANNEL 4
#define BUZZER_RESOLUTION 8
#define BUZZER_FREQUENCY 2000

#define BUZZER_SUCCESS_FREQ 800
#define BUZZER_FAIL_FREQ 200
#define BUZZER_KEY_FREQ 500

#define BUZZER_KEY_DURATION 100
#define BUZZER_SUCCESS_DURATION 3000
#define BUZZER_FAIL_DURATION 3000

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

void setup()
{
    Serial.begin(115200);

    // setupBuzzer();

    // setupServo();

    // setupRelay();

    // setupPhotoresistor();

    setupKeypad();
}

void loop()
{
    testKeypad();
}

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
    servo.write(180);
    delay(3000);
    servo.write(0);
    delay(3000);
}

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