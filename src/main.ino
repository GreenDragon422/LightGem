#include <arduino.h>

int Led = 13;// define LED Interface
int touchSensorValue = 0;
int touchSensorInputPin = 3;
int ledPin =8;

long lastDebounceTime = 0;
unsigned long debounceDelay = 200;
int sensorState = 0;
int lastSensorState = 0;

bool isTouched;
int previousIsTouched = 0;

bool CurrentTouchState();

void setup ()
{
    Serial.begin(9600);
    pinMode(ledPin,OUTPUT);
    pinMode (LED_BUILTIN, OUTPUT) ;
    digitalWrite(LED_BUILTIN,LOW);
    pinMode (touchSensorInputPin, INPUT) ;
}

void loop ()
{
    isTouched = CurrentTouchState();

    if (isTouched && !previousIsTouched)
    {
        for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) {
            analogWrite(ledPin, fadeValue);
            delay(30);
        }
    }
    else if (!isTouched && previousIsTouched)
    {
        for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) {
            analogWrite(ledPin, fadeValue);
            delay(30);
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
            }
            else
            {
                isTouched = false;
            }
            Serial.println(sensorState);
        }
    }

    return isTouched;
}

