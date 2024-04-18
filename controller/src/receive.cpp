#include <Arduino.h>
// Receiver code
#include <SoftwareSerial.h>
SoftwareSerial link(3, 5); // Rx, Tx

void setup()
{
    link.begin(9600);     // setup software serial
    Serial.begin(115200); // setup serial monitor
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.print("reset...");

    pinMode(3, INPUT);
    pinMode(5, OUTPUT);
}

void loop()
{
    String content = "";
    char character;

    while (link.available())
    {
        character = link.read();
        content.concat(character);
    }

    if (content != "")
    {
        Serial.print(content);
    }
}
