#!/bin/bash

feed=$(curl -s https://openbenches.org/feed)

# get historical value from any query string value
historical=$(echo "${QUERY_STRING}" | awk -F '=|&' '{print $2}')
if [ -z $historical ]; then
  historical="1"
fi

echo "Content-type: text/plain"
echo "Last-Modified: " $(echo $feed | pcregrep -o1 '<pubDate>(.*?)</pubDate>' | head -n1)
echo ""

echo $feed | tr "\\n" " " | tr -d $'\r' | xmlstarlet sel -T -t -v '(/rss/channel/item/description)['"${historical}"']' /dev/stdin | sed 's/<br><img src.*//g' | perl -n -mHTML::Entities -e ' ; print HTML::Entities::decode_entities($_) ;' | sed 's/^\s*//' | sed 's/\s*$//'

exit 0
