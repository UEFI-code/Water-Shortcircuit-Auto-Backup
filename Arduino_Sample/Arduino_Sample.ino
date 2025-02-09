#define threshold 300

void setup()
{
    // set A0 as input
    pinMode(A0, INPUT);
    Serial.begin(9600);
}

int sensorValue = 0;
char StatusBuffer[16] = {'I', 'D', 'L', 'E', ' ', 0};

void loop()
{
    sensorValue = analogRead(A0);
    if (sensorValue < threshold) // assume shortcut to ground
    {
        Serial.print("ALERT");
    }
    else
    {
        itoa(sensorValue, StatusBuffer+5, 10);
        Serial.print(StatusBuffer);
    }
    delay(1000);
}