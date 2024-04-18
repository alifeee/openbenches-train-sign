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
#include <SPI.h>

#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include <secrets.h>

void setup()
{
    // SETUP SERIAL
    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }

    // SETUP SPI
    SPI.begin();

    // SETUP wifi
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
    wifiMulti.addAP(WIFI_SSID_2, WIFI_PASSWORD_2);
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

            // send over SPI
            SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
            char c;
            for (const char *p = payload.c_str(); c = *p; p++)
            {
                SPI.transfer(c);
            }
            // send message terminator? or it might be in the payload above
            SPI.endTransaction();
        }
        else
        {
            Serial.print("Error code: ");
            Serial.println(httpCode);
        }

        http.end();
    }
    delay(5000);
}