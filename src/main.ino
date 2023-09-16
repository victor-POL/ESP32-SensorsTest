#include <Arduino.h>
#include <ESP32Servo.h>

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

void setup()
{
    Serial.begin(115200);

    // setupBuzzer();

    // setupServo();

    setupRelay();
}

void loop()
{
    testRelay();
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