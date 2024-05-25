#include <Arduino.h>
// Receiver code
#include <SoftwareSerial.h>
SoftwareSerial link(3, 5); // Rx, Tx

#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <BigClock.h>

#include <graphics.cpp>

GFXcanvas1 *canvas = NULL;
BigClock *bc = NULL;

String message;

void draw(String);

void setup()
{
    link.begin(9600);     // setup software serial
    Serial.begin(115200); // setup serial monitor
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.print("reset...");

    pinMode(3, INPUT);
    pinMode(5, OUTPUT);

    canvas = new GFXcanvas1(96, 26);
    bc = new BigClock();
    bc->init();
}

void loop()
{
    char character;

    while (link.available())
    {
        character = link.read();
        message.concat(character);
        if (character == '\0')
        {
            Serial.println("got message");
            Serial.println(message);
            draw(message);
            message = "";
        }
    }
}

void draw(String m)
{
    drawText(canvas, m.c_str());

    uint8_t *buffer = canvas->getBuffer();

    bc->output(buffer);
}
