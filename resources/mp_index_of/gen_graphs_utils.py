#!/bin/python
import matplotlib.pyplot as plt
import numpy as np
import subprocess

plt.style.use('my.mplstyle')

def create_graph(title, algos, datas, xlegends, ylabel):
  dim = len(algos)
  w = 0.75
  dimw = w / dim
  fig, ax = plt.subplots()
  ax.set_title(title)
  x = np.arange(len(datas))
  for i in range(len(datas[0])):
    y = [d[i] for d in datas]
    b = ax.bar(x + i * dimw, y, dimw, bottom=0)

  # ax.set_xticklabels([e[0] for e in elems])
  # for tick in ax.get_xticklabels():
  #  tick.set_rotation(55)

  # ax.set_xticks(w/len(algos) + np.arange(len(elems)))

  #ax.set_yscale('log')

  # ax.set_xlabel('algos')
  ax.set_ylabel(ylabel)

  plt.legend(algos)
  plt.subplots_adjust(bottom=0.33, right=0.95, top=.94)
  # plt.xticks(dimw*dim/2 + np.arange(len(xlegends)), xlegends, rotation='vertical')
  plt.xticks((dim-1)*0.5*dimw + np.arange(len(xlegends)), xlegends, rotation=55)
  return plt

def gen_graphs(pngprefixname, algos, elems, repeat=5, compiler='g++'):
  xlegends = [e[0] for e in elems]
  ylegend_time = 'durée de compilation en seconde'
  ylegend_memory = 'mémoire utilisée en méga octet'

  create_graph(title, algos, data_times, xlegends, ylegend_time).\
    savefig(pngprefixname + '-time.png')
  create_graph(title, algos, data_memories, xlegends, ylegend_memory).\
    savefig(pngprefixname + '-memory.png')

  times,memories = [],[]
  compiler_flags = sorted(set(sum([e[1] for e in elems], [])))
  for algo in algos:
    gtimes, gmemories, count = create_datas(algo, repeat, compiler, compiler_flags, count, n)
    times.append(gtimes[imedian])
    memories.append(gmemories[imedian])
  data_times.append(times)
  data_memories.append(memories)

  xlegends.append('all')
  create_graph(title, algos, data_times, xlegends, ylegend_time).\
    savefig(pngprefixname + '-time-all.png')
  create_graph(title, algos, data_memories, xlegends, ylegend_memory).\
    savefig(pngprefixname + '-memory-all.png')
