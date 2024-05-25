# Transformer

For this project, I only want "the inscription of the most recent bench". OpenBenches provides [a geoJSON API][OpenBenches API], but I can't see a way to limit this to "most recent 1/5/10 benches" (see [GitHub issue](https://github.com/openbenches/openbenches.org/issues/364)), so I use the [RSS feed][OpenBenches RSS Feed].

[OpenBenches RSS Feed]: https://openbenches.org/feed

The CGI scripts in this folder parse the RSS feed and returns an HTTP request with the text of the most recently uploaded bench. They also include a `Last-Modified` header using the RSS `pubDate`. It gets the description from the RSS `description`, and returns it, with newlines replaced with space `" "`, HTML entities decoded (i.e., `&#039;` `&amp;` `&quot;` → `'` `&` `"`), and whitespace trimmed.

This CGI script can be placed on an HTTP server by using nginx with `fastcgi`. First, place the files and install `xmlstarlet`.

```bash
mkdir -p /var/www/cgi/bench/
cp ./transformer/* /var/www/cgi/bench/
sudo apt-get update
sudo apt-get install xmlstarlet
```

You can test them locally by running them as executable files. You can first see the headers which will be sent, and the body after a newline.

```bash
$ ./full.cgi
Content-type: text/plain
Last-Modified:  Fri, 24 May 2024 22:57:41 +0100

In loving memory of
Doreen McKenzie
1928 - 2021
A local resident for 58 years
she loved this park.
```

Here is an example nginx config. When debugging, running `sudo strace -f -e trace=file -p $(pidof fcgiwrap)` will display all attempts to do things by `fastcgi`, which is very helpful.

```nginx
server {
    ...
    location /bench/ {
        fastcgi_intercept_errors on;
        include fastcgi_params;
        fastcgi_param SCRIPT_FILENAME /var/www/cgi/$fastcgi_script_name;
        fastcgi_pass unix:/var/run/fcgiwrap.socket;
    }
    ...
}
```

For example, I have placed them on:

- <https://server.alifeee.co.uk/bench/line.cgi>
- <https://server.alifeee.co.uk/bench/full.cgi>

```bash
$ curl -s https://server.alifeee.co.uk/bench/full.cgi
In loving memory of
Doreen McKenzie
1928 - 2021
A local resident for 58 years
she loved this park.
```

[OpenBenches API]: https://github.com/openbenches/openbenches.org?tab=readme-ov-file#open-data-api
