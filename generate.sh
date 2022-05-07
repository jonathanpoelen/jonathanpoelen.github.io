#!/bin/sh

# ./generate.sh server --buildDrafts

cd "$(dirname "$0")"

#export PATH="$PWD:$PATH"
exec hugo "$@"
#exec hugo "$@" --theme=mainroad
