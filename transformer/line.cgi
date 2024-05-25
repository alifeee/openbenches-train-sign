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

CACHE="feed.cache"

get_and_save() {
  curl -s https://openbenches.org/feed | tee $CACHE
}

# cache with TTL value from feed
if [ -e $CACHE ]; then
  feed=$(cat $CACHE)
  ttl_min=$(echo "${feed}" | xmlstarlet sel -T -t -v "/rss/channel/ttl" /dev/stdin)
  echo "cache found. TTL is $ttl_min min, checking age..." > /dev/stderr
  if [ $(( `date +%s` - `stat --format %Y feed.cache` )) -gt $(($ttl_min * 60)) ]; then
    echo "too old... re-fetching..." > /dev/stderr
    feed=$(get_and_save)
  else
    echo "cache is ok to use!" > /dev/stderr
  fi
else
  echo "no cache file :/" > /dev/stderr
  feed=$(get_and_save)
fi
echo "" > /dev/stderr

# get historical value from any query string value
historical=$(echo "${QUERY_STRING}" | awk -F '=|&' '{print $2}')
if [ -z $historical ]; then
  historical="1"
fi
h=$historical

echo "Content-type: text/plain"
echo "Last-Modified: " $(echo $feed | xmlstarlet sel -T -t -v "/rss/channel/pubDate" /dev/stdin)
echo "Bench-URL: " $(echo $feed | xmlstarlet sel -T -t -v "(/rss/channel/item/link)[${h}]" /dev/stdin)
echo ""

echo "${feed}" | tr "\\n" " " | tr -d $'\r' | xmlstarlet sel -T -t -v '(/rss/channel/item/description)['"${historical}"']' /dev/stdin | sed 's/<br><img src.*//g' | perl -n -mHTML::Entities -e ' ; print HTML::Entities::decode_entities($_) ;' | sed 's/^\s*//' | sed 's/\s*$//'
echo ""

exit 0
