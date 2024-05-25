/**
 * @file get_http.cpp
 * @author alifeee (alifeee.web@outlook.com)
 * @brief Fetches text from an HTTP endpoint and sends it over SPI
 * @version 0.1
 * @date 2024-04-17
 *
 * @copyright Use freely
 */
#include <Arduino.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

// serial
#include <SoftwareSerial.h>
#define RX 4
#define TX 5
SoftwareSerial link(RX, TX); // Rx, Tx

#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <secrets.h>

void setup()
{
    // SETUP SERIAL
    Serial.begin(9600);
    while (!Serial)
    {
        delay(100);
    }
    Serial.print("Set up!");

    // SETUP INTRA-DEVICE SERIAL
    link.begin(9600);
    pinMode(RX, INPUT);
    pinMode(TX, OUTPUT);

    // SETUP wifi
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED)
    {
        Serial.println("Wifi connection lost");
    }
    else
    {
        WiFiClient client;
        HTTPClient http;
        Serial.print("[HTTP] begin...\n");

        http.begin(client, FILE_URL);

        Serial.printf("[HTTP] GET... %s\n", FILE_URL);

        int httpCode = http.GET();

        if (httpCode > 0)
        {
            Serial.print("HTTP Response code: ");
            Serial.println(httpCode);
            String payload = http.getString();
            Serial.println(payload);

            // send over Serial
            link.print(payload);
            link.print('\0');
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpCode);
        }

        http.end();
    }
    delay(10000);
}
