#!/bin/python
import gen_datas_utils
import sys

algos1 = (
  'recursive',
  'recursive_bool',
  'recursive_bool2',
  'recursive_bool3',
  'recursive_bool_ternary',
  'recursive_indexed',
  'recursive_ternary',
  'by_indices',
  'pack8',
  'pack8_bool',
  'pack8_bool2',
)
algos2 = (
  'loop',
  'loop2',
  'loop_rec',
  'loop_rec2',
  'loop_rec3',
  'fold',
  'fold2',
)

nargs = len(sys.argv)
if nargs <= 3:
  if nargs == 2 and sys.argv[1] == '-l':
    print('\n'.join(algos1), '\n'.join(algos2), sep='\n')
    exit(0)
  print(sys.argv[0], '-l', file=sys.stderr)
  print(sys.argv[0], '{depth} {compiler} {repeat} {algo|onlyall|all}[,...]', file=sys.stderr)
  exit(1)


depth = int(sys.argv[1])
compiler = sys.argv[2]
repeat = int(sys.argv[3])
algos = None
if nargs > 4:
  algos = sys.argv[4].split(',')

fdepth = [f'-ftemplate-depth={max(depth+100, 1000)}', '-fconstexpr-depth=1500']

data_options = [
  ('[]', ['-DTEST_EMPTY']),

  (f'[x*{depth}]', [f'-DTEST_N1={depth}', '-DTEST_F=0'] + fdepth),
  (f'[_*{depth//2}, x, _*{depth//2-1}]', [f'-DTEST_N1={depth}', '-DTEST_F=1'] + fdepth),
  (f'[_*{depth//2}, x*{depth//2}]', [f'-DTEST_N1={depth}', '-DTEST_F=2'] + fdepth),
  (f'[_*{depth-1}, x]', [f'-DTEST_N1={depth}', '-DTEST_F=3'] + fdepth),
  (f'[_*{depth}]', [f'-DTEST_N1={depth}', '-DTEST_F=4'] + fdepth),

  ('n=100\n[i{0..n},x,i{n-(n..0)}]', ['-DTEST_S1=100']),

  ('[i{0..40}*150]', ['-DTEST_L1=150', '-DTEST_LN=40']),

  ('[i{0..120}..i120, x]', ['-DTEST_SI1=120']),
  ('[i0..i{0..120}, x]', ['-DTEST_SD1=120']),
]


all_algo = 0
selected_algos = []
if algos:
  for a in algos:
    if a == '1':
      selected_algos += algos1
    elif a == '2':
      selected_algos += algos2
    elif a == 'all':
      all_algo = 1
    elif a == 'onlyall':
      all_algo = 2
    elif a in algos1:
      selected_algos.append(a)
    elif a in algos2:
      selected_algos.append(a)
    elif a == '*':
        selected_algos += algos1
        selected_algos += algos2
    else:
      print(f'Unknown algo {a}', file=sys.stderr)
      exit(2)
else:
  selected_algos = algos1 + algos2
  all_algo = 1

selected_algos = list(set(selected_algos))

indexed_data_options = [x for x in enumerate(data_options)]
if all_algo:
  new_e = (len(data_options), ('all', list(set(sum([f for (l,f) in data_options], [])))))
  if all_algo == 1:
    indexed_data_options.append(new_e)
  else:
    indexed_data_options = [new_e]


elems = []

for a in selected_algos:
  extra = ['-DNAMESPACE='+a]
  for (i,d) in indexed_data_options:
    elems.append((a, i, d[0], extra + d[1]))


datas = gen_datas_utils.gen_datas(elems, depth, repeat, compiler)
print(datas)
