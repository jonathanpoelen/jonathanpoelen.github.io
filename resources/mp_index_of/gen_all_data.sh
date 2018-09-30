#!/bin/bash

set -e

n=${1:-1200}
algos=$2
suffix=
[ -z "$algos" ] \
&& algos=$(./gen_datas.py -l | sed /^loop_rec/d | sed ':s;N;s/\n/,/;bs') \
|| suffix="-$algos"

for c in g++ g++-7 clang++ ; do
  ./gen_datas.py "$n" "$c" 11 "$algos,all"
done | ./concat_datas.sh > datas_"$n$suffix".py
