#include <arduino.h>
#include <LiquidCrystal.h>

const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char fadeValueBuffer[50];
char touchBuffer[50];

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

    if (isTouched && !previousIsTouched)
    {
        lcd.setCursor(0,0);
        sprintf(touchBuffer, "%-16s", "Touched");
        lcd.print(touchBuffer);
    }
    else if (!isTouched && previousIsTouched)
    {
        lcd.setCursor(0,0);
        sprintf(touchBuffer, "%-16s", "Not Touched");
        lcd.print(touchBuffer);
    }

    if (isTouched && !previousIsTouched)
    {
        for(int fadeValue = 0 ; fadeValue <= 255; fadeValue++) {
            analogWrite(ledPin, fadeValue);
            delay(delayTimeMs);
            Serial.println("Light: " + String(fadeValue));

            sprintf(fadeValueBuffer, "Led Value: %3d", fadeValue);
            lcd.setCursor(0,1);
            lcd.print(fadeValueBuffer);
        }
    }
    else if (!isTouched && previousIsTouched)
    {
        for(int fadeValue = 255 ; fadeValue >= 0; fadeValue--) {
            analogWrite(ledPin, fadeValue);
            delay(delayTimeMs);
            Serial.println("Dark: " + String(fadeValue));

            sprintf(fadeValueBuffer, "Led Value: %3d", fadeValue);
            lcd.setCursor(0,1);
            lcd.print(fadeValueBuffer);
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
