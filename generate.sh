#!/bin/bash

# ./generate.sh server --buildDrafts

set -e

cd "$(dirname "$0")"

#export PATH="$PWD:$PATH"
hugo "$@"
#hugo "$@" --theme=my

if (( $# == 0 )) ; then
  # fix ```
  find public \( -path 'public/2???/*' -or -path 'public/page/*' \) -type f -name '*.html' -exec \
    sed -i '/<pre tabindex="0"><code>/,/<\/code><\/pre>/{s#<pre tabindex="0"><code>#<div class="highlight-nocode"><pre tabindex="0" class="chroma-nocode"><code>#;N;s#^</code></pre>#</code></pre></div>#}' '{}' +
fi
