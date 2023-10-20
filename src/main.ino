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
int curLedValue = 0;  // Current brightness value of the led
int targetLedValue = 0; // Target brightness value of the led

enum State
{
    UNKNOWN,
    TOUCHED,
    NOT_TOUCHED,
    DIM
} currentState = UNKNOWN;

bool isSensorTouched();

void updateLed()
{
    if(curLedValue != targetLedValue )
    {
        curLedValue += targetLedValue > curLedValue ? 1 : -1;
        analogWrite(ledPin, curLedValue);
        sprintf(fadeValueBuffer, "Led Value: %3d", curLedValue);
        lcd.setCursor(0, 1);
        lcd.print(fadeValueBuffer);
    }
}

void DisplayMessage(const char* message)
{
    lcd.setCursor(0, 0);
    sprintf(touchBuffer, "%-16s", message);
    lcd.print(touchBuffer);
}

void setup() {
    pinMode(ledPin, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    pinMode(touchSensorInputPin, INPUT);
    lcd.begin(16, 2);
}

void loop ()
{
    static unsigned long dimMillis; // Add a variable to hold the time when 'Dim' state is entered

    State previousState = currentState;
    if (isSensorTouched())
    {
        currentState = TOUCHED;
        targetLedValue = 255;
    }
    else if(previousState == TOUCHED)
    {
        currentState = DIM;
        targetLedValue = int(255 * 0.75);
        dimMillis = millis(); // Save the time when 'Dim' state is entered
    }
    else if(previousState == DIM)
    {
        if (millis() - dimMillis >= 60000) // Wait for one minute before changing from 'Dim' to 'Untouched'
        {
            currentState = NOT_TOUCHED;
            targetLedValue = 0;
        }
    }

    updateLed();

    switch (currentState)
    {
        case TOUCHED:
        {
            DisplayMessage("Touched");
            break;
        }
        case NOT_TOUCHED:
        {
            DisplayMessage("Not Touched");
            break;
        }
        case DIM:
        {
            DisplayMessage("Dim");
            lcd.setCursor(14, 0);
            lcd.print((int)((millis() - dimMillis) / 1000));  // Display time passed since 'Dim' state was entered
            break;
        }
        default:
            break;
    }

    // Wait for led brightness updating
    delay(delayTimeMs);}

bool isSensorTouched() {
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
        }
    }

    return isTouched;
}