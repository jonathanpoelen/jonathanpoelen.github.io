#!/bin/sh

set -e

cd "$(dirname "$0")"

./generate.sh

cd public
git add .
git commit -m "Site updated at $(LANG=en date --rfc-3339=seconds -u)"
git push

[ -z "$@" ] && wget \
  'www.google.com/webmasters/tools/ping?sitemap=http://jonathanpoelen.github.io/sitemap.xml' \
  'www.bing.com/webmaster/ping.aspx?siteMap=http://jonathanpoelen.github.io/sitemap.xml' \
  -O /dev/null
