#include <arduino.h>
#include <LiquidCrystal.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char buffer[50];

int touchSensorValue = 0;
int touchSensorInputPin = 11;
int ledPin = 12;

long lastDebounceTime = 0;
unsigned long debounceDelay = 200;
int sensorState = 0;
int lastSensorState = 0;

bool isTouched;
int previousIsTouched = 0;
int transitionTimeSec = 1;   // transition time in seconds
int delayTimeMs = transitionTimeSec * 1000 / 255; // compute the delay in milliseconds based on the transition time

bool CurrentTouchState();

void setup() {
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(touchSensorInputPin, INPUT);
    lcd.begin(16, 2);
}

void loop ()
{
    isTouched = CurrentTouchState();

    lcd.setCursor(0,0);

    if (isTouched && !previousIsTouched)
    {
        lcd.clear();
        lcd.print("Touched");
    }
    else if (!isTouched && previousIsTouched)
    {
        lcd.clear();
        lcd.print("Not Touched");
    }

    if (isTouched && !previousIsTouched)
    {
        for(int fadeValue = 0 ; fadeValue <= 255; fadeValue++) {
            analogWrite(ledPin, fadeValue);
            delay(delayTimeMs);
            Serial.println("Light: " + String(fadeValue));

            sprintf(buffer, "Led Value: %3d", fadeValue);
            lcd.setCursor(0,1);
            lcd.print(buffer);
        }
    }
    else if (!isTouched && previousIsTouched)
    {
        for(int fadeValue = 255 ; fadeValue >= 0; fadeValue--) {
            analogWrite(ledPin, fadeValue);
            delay(delayTimeMs);
            Serial.println("Dark: " + String(fadeValue));

            sprintf(buffer, "Led Value: %3d", fadeValue);
            lcd.setCursor(0,1);
            lcd.print(buffer);
        }
    }

    previousIsTouched = isTouched;
}

bool CurrentTouchState() {
    int currentSensorValue = digitalRead(touchSensorInputPin);

    if (touchSensorValue != lastSensorState)
    {
        lastDebounceTime = millis();
    }

    touchSensorValue = currentSensorValue;

    if (millis() - lastDebounceTime > debounceDelay) {
        if (currentSensorValue != sensorState) {
            sensorState = currentSensorValue;

            if (sensorState == 1)
            {
                isTouched = true;
            } else
            {
                isTouched = false;
            }
            Serial.println(sensorState);
        }
    }

    return isTouched;
}
