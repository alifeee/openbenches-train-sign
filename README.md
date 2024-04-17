# OpenBenches Train Sign

Attempt to display the most recent bench from <https://openbenches.org/> on a [train sign].

There are three parts

- a script to turn the [OpenBenches RSS Feed] into the text for the most recent bench
- an ESP8266 microcontroller to fetch the latest bench, and push it over I2C
- an Arduino to control the text on the sign, and listen on I2C for new text to display

[train sign]: https://github.com/sheffieldhackspace/train-signs
[OpenBenches RSS Feed]: https://openbenches.org/feed
