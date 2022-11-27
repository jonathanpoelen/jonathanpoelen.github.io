#!/bin/bash

set -e

if [[ -z "$*" ]] ; then
  read -p 'Title: ' title
else
  title="$*"
fi

ext=.md

cd "$(dirname "$0")"

filename="${title,,}"
filename="${filename//,/}"
filename="${filename//\?/}"
filename="${filename// /-}"
filename="${filename//\//-}"
filename="${filename//\'/-}"
filename="${filename//\"/-}"
filename="${filename//é/e}"
filename="${filename//è/e}"
filename="${filename//ê/e}"
filename="${filename//à/a}"
filename="${filename//ù/u}"
filename="${filename//:/}"

base=content/post
oldpath="$base/$title"$ext
newpath="$base/$filename"$ext

if [[ -f "$newpath" ]] || [[ -f "$oldpath" ]] ; then
  echo Error: "\"$title\"" already exists.
  exit 255
fi

hugo new post/"$title"$ext >/dev/null

if [[ "$oldpath" != "$newpath" ]] ; then
  mv "$oldpath" "$newpath"
  sed 's/^#slug:.*/slug: "'"$filename"'"/' -i -- "$newpath"
fi

echo "$newpath"

exec "${EDITOR:-nano}" "$newpath"
