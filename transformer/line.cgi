#!/bin/bash
# get the most recent bench inscription from
#  https://openbenches.org/feed
#  with only one terminating newline
# requires xmlstarlet, install with
#  apt-get install xmlstarlet
# run as script:
#  ./line.cgi
#  QUERY_STRING="n=3" ./line.cgi
# run as CGI script with nginx
#  > must edit NGINX config... see README...

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
echo ""

exit 0
