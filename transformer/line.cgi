#!/bin/bash

feed=$(curl -s https://openbenches.org/feed)

echo "Content-type: text/plain"
echo "Last-Modified: " $(echo $feed | pcregrep -o1 '<pubDate>(.*?)</pubDate>' | head -n1)
echo ""

echo $feed | tr "\\n" " " | tr -d $'\r' | xmlstarlet sel -T -t -v '(/rss/channel/item/description)[1]' /dev/stdin | sed 's/<br><img src.*//g' | perl -n -mHTML::Entities -e ' ; print HTML::Entities::decode_entities($_) ;' | sed 's/^\s*//' | sed 's/\s*$//' | head -n1

exit 0
