#include <arduino.h>

#ifndef ARDUINO_AVR_ATTINYX5
#include <LiquidCrystal.h>
#else
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#endif

#ifndef ARDUINO_AVR_ATTINYX5
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
#else
hd44780_I2Cexp lcd(0,16,2);
#endif

char fadeValueBuffer[50];
char touchBuffer[50];

int touchSensorValue = 0;

#ifndef ARDUINO_AVR_ATTINYX5
int touchSensorInputPin = 11;
#else
int touchSensorInputPin = PB3;
#endif

#ifndef ARDUINO_AVR_ATTINYX5
int ledPin = 12;
#else
int ledPin = PB1;
#endif

long lastDebounceTime = 0;
unsigned long debounceDelay = 200;
int sensorState = 0;
int lastSensorState = 0;

bool isTouched;
int transitionTimeSec = 1;   // transition time in seconds
int delayTimeMs = transitionTimeSec * 1000 / 255; // compute the delay in milliseconds based on the transition time
int currentLedValue = 0;  // Current brightness value of the led
int targetLedValue = 0; // Target brightness value of the led
static unsigned long lastDimTimeUpdate = 0;  // holds the time of the last LCD update

// declare dimTimeMessage as a global variable
char dimTimeMessage[17];

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
    if(currentLedValue != targetLedValue )
    {
        currentLedValue += targetLedValue > currentLedValue ? 1 : -1;
        analogWrite(ledPin, currentLedValue);

        sprintf(fadeValueBuffer, "Led Value: %3d", currentLedValue);
        lcd.setCursor(0, 1);
        lcd.print(fadeValueBuffer);
    }
}

int digitCount(int number) {  // Helper function to calculate the number of digits in 'number'. Returns a digit count.
    int count = 0;
    while(number != 0) {
        number /= 10;
        ++count;
    }
    return count;
}

void DisplayMessage(const char* message)
{
    lcd.setCursor(0, 0);
    sprintf(touchBuffer, "%-16s", message);
    lcd.print(touchBuffer);
}

// Function to format the dim time message
void formatDimTimeMessage(int elapsedTime) {
    int secondStringLength = digitCount(elapsedTime);
    int spaceCount = 16 - 5 - secondStringLength;  // Compute available space, 16 total - "Dim: " 5 chars - width of seconds

    char spaceBuffer[spaceCount + 1];
    memset(spaceBuffer, ' ', spaceCount);
    spaceBuffer[spaceCount] = '\0';  // don't forget to terminate the string
    sprintf(dimTimeMessage, "Dim: %s%d", spaceBuffer, elapsedTime);
}

void setup() {
    pinMode(ledPin, OUTPUT);
#ifndef ARDUINO_AVR_ATTINYX5
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
#endif

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
        targetLedValue = int(255 * 0.50);
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
            unsigned long now = millis();

            if (now - lastDimTimeUpdate >= 1000) {  // check if at least one second has passed
                int elapsedTime = (int) ((now - dimMillis) / 1000);

                formatDimTimeMessage(elapsedTime);
                lcd.setCursor(0, 0);
                lcd.print(dimTimeMessage);  // Display composed message

                lastDimTimeUpdate = now;  // update the time of the last LCD update
            }
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