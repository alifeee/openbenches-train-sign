#include <Arduino.h>
// Transmitter Code
#include <SoftwareSerial.h>
SoftwareSerial link(3, 1); // Rx, Tx

String f;

void setup()
{
    link.begin(9600);
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(3, INPUT);
    pinMode(1, OUTPUT);
}

void loop()
{
    f = String("In loving memory of DR. DHIRENDRA M. GOSALIA Who devoted his life to caring for the poor of Mumbai, India. 26/11/1941-25/2/2016");
    link.println(f);

    delay(2000);
}
