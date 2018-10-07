#!/bin/sh

# ./generate.sh server --buildDrafts

cd "$(dirname "$0")"

#export PATH="$PWD:$PATH"
exec hugo "$@" --i18n-warnings
#exec hugo "$@" --theme=mainroad --i18n-warnings
