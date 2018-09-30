#!/bin/sh

echo 'datas = ['
sed 's/),/),\n/g;s/^\[(/ (/;s/)]$/),/' "$@"
echo ']'
