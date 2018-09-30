#!/bin/bash

{
  ./gen_datas.py 1400 g++ 11 '*,all'
  ./gen_datas.py 1400 g++-7 11 '*,all'
  ./gen_datas.py 1200 clang++ 11 1,all
  ./gen_datas.py 700 clang++ 11 2,all
} | ./concat_datas.sh
