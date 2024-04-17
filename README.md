# OpenBenches Train Sign

Attempt to display the most recent bench from <https://openbenches.org/> on a [train sign].

There are three parts

- (transformer) a script to turn the [OpenBenches RSS Feed] into the text for the most recent bench
- (fetcher) an ESP8266 microcontroller to fetch the latest bench, and push it over I2C
- (controller) an Arduino to control the text on the sign, and listen on I2C for new text to display

[train sign]: https://github.com/sheffieldhackspace/train-signs
[OpenBenches RSS Feed]: https://openbenches.org/feed

## Transformer

For this project, I only want "the inscription of the most recent bench". OpenBenches provides [a geoJSON API][OpenBenches API], but I can't see a way to limit this to "most recent 1/5/10 benches" (see [GitHub issue](https://github.com/openbenches/openbenches.org/issues/364)), so I use the [RSS feed][OpenBenches RSS Feed].

This bash CGI script parses the RSS feed and returns an HTTP request with the text of the most recently uploaded bench. It also includes the `Last-Modified` header using the RSS `pubDate`. It gets the description from the RSS `description`, and returns it, with newlines replaced with space `" "`, HTML entities decoded (i.e., `&#039;` `&amp;` `&quot;` → `'` `&` `"`), and whitespace trimmed.

```bash
#!/bin/bash

feed=$(curl -s https://openbenches.org/feed)

echo "Content-type: text/plain"
echo "Last-Modified: " $(echo $feed | pcregrep -o1 '<pubDate>(.*?)</pubDate>' | head -n1)
echo ""

echo $feed | tr "\\n" " " | tr -d $'\r' | pcregrep -Mo1 '<description><!\[CDATA\[((\n|.)*?)<br>(\n|.)*?</description>' | perl -n -mHTML::Entities -e ' ; print HTML::Entities::decode_entities($_) ;' | sed 's/^\s*//' | sed 's/\s*$//' | head -n1

exit 0
```

This CGI script can be placed on an HTTP server by using this nginx config. When debugging `SCRIPT_FILENAME`, running `sudo strace -f -e trace=file -p $(pidof fcgiwrap)` will display all attempts to do things by `fastcgi`, which is very helpful.

```nginx
server {
    ...
    location /bench {
        fastcgi_intercept_errors on;
        include fastcgi_params;
        fastcgi_param SCRIPT_FILENAME /var/www/localhost/bench.cgi;
        fastcgi_pass unix:/var/run/fcgiwrap.socket;
    }
    ...
}
```

For example, I have placed it on <http://server.alifeee.co.uk/bench>

```text
IN MEMORY OF FRED KENYON 1927 TO 2012 Railwayman for over 40 years Love from all his Family
```

[OpenBenches API]: https://github.com/openbenches/openbenches.org?tab=readme-ov-file#open-data-api

## Fetcher

This is code to flash an ESP8266, in [`./fetcher/`](./fetcher/).

To program the ESP, use [Platform IO for VSCode](https://docs.platformio.org/en/latest/integration/ide/vscode.html) or another method. To use PlatformIO, you must use VSCode.

Copy the example secrets file ([`./fetcher/src/secrets.example.h`](./fetcher/src/secrets.example.h)) and enter your Wi-Fi details, and the URL of the file to fetch (i.e., <http://server.alifeee.co.uk/bench>).
