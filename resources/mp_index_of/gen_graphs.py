#!/bin/python
from orderedset import OrderedSet
import gen_graphs_utils
import sys

# ./gen_graphs.py compiler=clang++,g++-7 algo=fold

def string(x):
  c = x[-1]
  if c == '?':
    return DigitSuffix(x[:-1])
  if c == '*':
    return AnySuffix(x[:-1])
  return x

field_pairs = (
  ('compiler', string, ','),
  ('version', string, ';'),
  ('depth', int, ','),
  ('algo', string, ','),
  ('repeat', int, ','),
  ('dataid', int, ','),
  ('legend', string, ';'),
  ('flags', string, ','),
  ('time', float, ','),
  ('memory', float, ','),
)
fields = [x[0] for x in field_pairs]
icompiler = 0
iversion = 1
ialgo = 3
idataid = 5
ilegend = 6
itime = 8
imemory = 9


class AnySuffix:
  def __init__(self, s):
    self.s = s

  def __eq__(self, s):
    return s.startswith(self.s)


class DigitSuffix:
  def __init__(self, s):
    self.s = s

  def __eq__(self, s):
    if not s.startswith(self.s):
      return False
    n = len(self.s)
    return len(s) == n or s[n:].isdigit()


def rng(i, x, y):
  return lambda t: x <= t[i] <= y

def eq(i, x):
  return lambda t: x == t[i]

def ge(i, x):
  return lambda t: x >= t[i]

def le(i, x):
  return lambda t: x <= t[i]

class Filters:
  def __init__(self):
    self.filters = []
    self.algo_filters = []
    self.compiler_filters = []

  def add_filter(self, i, x):
    field = field_pairs[i]
    converter = field[1]
    xs = x.split(field[2])
    includes, excludes = self.create_filter(i, converter, xs)
    self.filters.append(lambda t: (not includes or any(f(t) for f in includes)) \
                                  and not any(f(t) for f in excludes))

    if i == ialgo:
      self.algo_filters += includes
    elif i == icompiler:
      self.compiler_filters += includes

  def create_filter(self, i, converter, xs):
    includes = []
    excludes = []

    for x in xs:
      c = x[0]
      l = includes
      if c == '~':
        x = x[1:]
        c = x[0]
        l = excludes

      if c == '+':
        l.append(ge(i, converter(x)))
      elif c == '-':
        l.append(le(i, converter(x)))
      elif converter != string and '-' in x:
        both = x.split('-')
        l.append(rng(i, converter(both[0]), converter(both[1])))
      else:
        l.append(eq(i, converter(x)))

    return includes, excludes

  def isempty(self):
    return not bool(self.filters)

  def check(self, t):
    return all(f(t) for f in self.filters)


filters = Filters()
showhelp = None

nargs = len(sys.argv)

i = 1
while i < nargs:
  arg = sys.argv[i]
  if arg == '--':
    i = i+1
    break

  l = arg.split('=')
  index = fields.index(l[0])
  if index < 0:
    if not showhelp:
      showhelp = []
    showhelp.append(i)
    continue
  filters.add_filter(index, l[1])
  i = i+1

title = None
pngprefix = 'a'
if i < nargs:
  pngprefix = sys.argv[i]
  i = i+1

if i < nargs:
  print('load', sys.argv[i])
  with open(sys.argv[i]) as f:
    exec(f.read())
else:
  from datas.blog import datas


if showhelp or filters.isempty():
  if showhelp:
    for i in showhelp:
      print(f'Unknown filter {sys.argv[i]} at position {i}', file=sys.stderr)
  print(sys.argv[0], """{filter=[~]value[{','|';'}...]}... -- [pngprefix [datas.py]]

format for a numeric value:
  `n`: exactly equal to `n`.
  `+n`: greater or equal to `n`.
  `-n`: lower or equal to `n`.
  `n1-n2`: interval between `n1` and `n2` include.

format for a string value:
  `str*`: starts with `str`.
  `str?`: starts with `str` followed by 0 or more digit.

A format value starting with `~` reverses the condition.

filters:""", file=sys.stderr)
  for (name,f,sep) in field_pairs:
    print(f'  {name}: {f.__name__}[{sep}...]', file=sys.stderr)
  algo_list = list(set([t[ialgo] for t in datas]))
  compiler_list = list(set([t[icompiler] for t in datas]))
  algo_list.sort()
  compiler_list.sort()
  algos = '\n  '.join(algo_list)
  compilers = '\n  '.join(compiler_list)
  print(f"""
example: {sys.argv[0]} compiler=clang++,g++,g++-7 algo=recursive,recursive_bool dataid=0,1 -- 1200 datas/1200.py

algos:
  {algos}
compilers:
  {compilers}""", file=sys.stderr)
  exit(1)


g = [t for t in datas if filters.check(t)]

if not g:
  print(f'Empty set', file=sys.stderr)
  exit(2)


def extract_i_set(i, filters):
  a = []
  for f in filters:
    a += [t[i] for t in g if f(t)]
  return list(OrderedSet(a))

algos = extract_i_set(ialgo, filters.algo_filters)
compilers = extract_i_set(icompiler, filters.compiler_filters)

if len(compilers) > 1:
  g = [list(x) for x in g]
  for d in g:
    d[ialgo] = f'{d[icompiler]} {d[ialgo]}'

  newalgos = []
  for a in algos:
    for c in compilers:
      newalgos.append(f'{c} {a}')
  algos = newalgos

  def key(a):
    return algos.index(a[ialgo]) * 10000 + compilers.index(a[icompiler]) * 100 + a[idataid]
else:
  def key(a):
    return algos.index(a[ialgo]) * 100 + a[idataid]

g = sorted(g, key=key)

x = g[0][idataid]
xlegends = [g[0][ilegend]]
for i in range(1, len(g)):
  y = g[i][idataid]
  if y <= x:
    break
  x = y
  xlegends.append(g[i][ilegend])

for x in g:
  print(x)

n = len(algos)
ndata = len(g)
nlegend = len(xlegends)
extract = lambda x: [[g[ii*nlegend+i][x] for ii in range(n)] for i in range(nlegend)]
data_times = extract(itime)
data_memories = extract(imemory)

if not title:
  import re
  re_vers = re.compile(R'(\d+(?:\.\d+){1,2})')
  def extract_version(d):
    return f'{d[icompiler]} ({re_vers.search(d[iversion])[1]})'
  if compilers:
    title = ' ; '.join([extract_version(g[i*nlegend]) for i in range(len(compilers))])
  else:
    title = extract_version(g[0])

for (i, suffix, d, ylegend) in (
  (0, 'time', data_times, 'durée de compilation en seconde'),
  (1, 'memory', data_memories, 'mémoire utilisée en méga octet'),
):
  gen_graphs_utils.create_graph(title, algos, d, xlegends, ylegend)\
    .savefig(f'{pngprefix}_{suffix}.png')
