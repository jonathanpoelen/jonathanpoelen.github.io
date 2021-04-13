#!/bin/sh

set -e

cd "$(dirname "$0")"

./generate.sh

# fix ```
find public \( -path 'public/2???/*' -or -path 'public/page/*' \) -type f -name '*.html' -exec \
  sed -i '/<pre><code>/,/<\/code><\/pre>/{s#<pre><code>#<div class="highlight-nocode"><pre class="chroma-nocode"><code>#;N;s#^</code></pre>#</code></pre></div>#}' '{}' +

cd public
git add .
git commit -m "Site updated at $(LANG=en date --rfc-3339=seconds -u)"
git push

[ -z "$@" ] && wget \
  'www.google.com/webmasters/tools/ping?sitemap=https://jonathanpoelen.github.io/sitemap.xml' \
  'www.bing.com/webmaster/ping.aspx?siteMap=https://jonathanpoelen.github.io/sitemap.xml' \
  -O /dev/null || :
