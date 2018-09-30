#!/bin/python
import subprocess
import sys

def compile(compiler, options=[]):
  return subprocess.run(
    ['/usr/bin/time', '--format="%E %M"',
     compiler, '-fsyntax-only', '-w', '-std=c++17', 'mp_index_of1.cpp'] + options,
    stderr=subprocess.PIPE,
    stdout=subprocess.PIPE,
    universal_newlines=True,
    check=True)

def create_datas(repeat, compiler, compiler_flags, count, n):
  gtimes,gmemories = [],[]
  imedian = repeat//2
  trace = f'{compiler} {compiler_flags}'
  for i in range(repeat):
    count += 1
    print(f"{count}/{n} | [{i+1}/{repeat}] {trace}", end=' ', file=sys.stderr)
    try:
      c = compile(compiler, compiler_flags)
    except subprocess.CalledProcessError as e:
      print(e.stderr, file=sys.stderr)
      raise e
    #r.check_returncode()
    rtime = c.stderr[1:-2]
    r = rtime.split(' ')
    gtimes.append(float(r[0].split(':')[1]))
    gmemories.append(int(r[1])/1000)
    print(f"{r[0]}s {r[1]}K", file=sys.stderr)
  gtimes.sort()
  gmemories.sort()
  return gtimes[imedian], gmemories[imedian]

def gen_datas(elems, depth, repeat=5, compiler='g++'):
  output_version = compile(compiler, ['--version']).stdout
  version = output_version[:output_version.index('\n')]
  print(output_version, file=sys.stderr)
  compile(compiler, ['-DTESTS'])

  count = 0
  datas = []
  n = len(elems) * repeat

  for e in elems:
    time, memory = create_datas(repeat, compiler, e[3], count, n)
    datas.append((compiler, version, depth, e[0], repeat, e[1], e[2], e[3], time, memory))
    count += repeat

  return datas
