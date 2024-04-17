# Openbenches RSS Microcontroller

Attempting to get the most recent item from the [OpenBenches](https://openbenches.org/) <https://openbenches.org/feed> and send it over SPI.

## Run HTTP Server

Run an HTTP server to test with Python with

```bash
cd test
python -m http.server 8080
```

## Edit secrets

Create a file `./src/secrets.h`. See [`secrets.example.h`](./src/secrets.example.h) for the format. This has Wi-Fi SSID/password and URL for the text on it.

## Programming

Using PlatformIO.
