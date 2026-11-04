import argparse, csv
from pathlib import Path
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser()
parser.add_argument('--results',type=Path,default=ROOT/'results'/'recorded')
parser.add_argument('--output',type=Path,default=ROOT/'figures'/'runtime-comparison.png')
args=parser.parse_args()
rows=list(csv.DictReader((args.results/'aggregate.csv').open(newline='')))
plt.rcParams.update({'font.family':'DejaVu Sans','font.size':10,'axes.titlesize':12,'axes.spines.top':False,'axes.spines.right':False})
fig,axes=plt.subplots(1,3,figsize=(7.6,3.6),sharey=True)
for ax,ratio in zip(axes,[4,16,64]):
    r=[v for v in rows if int(v['edge_to_vertex_ratio'])==ratio]
    x=[int(v['vertices']) for v in r]
    b=[float(v['binary_median_of_seed_medians_us'])/1000 for v in r]
    f=[float(v['fibonacci_median_of_seed_medians_us'])/1000 for v in r]
    ax.plot(x,b,'D-',color='#141414',lw=2,ms=4,label='James / binary')
    ax.plot(x,f,'o-',color='#E85B2A',lw=2,ms=4,label='Ben / Fibonacci')
    ax.set_title(f'E/V = {ratio}')
    ax.set_xticks(x,['250','1,000','4,000'])
    plt.setp(ax.get_xticklabels(),rotation=35,ha='right')
    ax.set_xlim(0,4300);ax.set_ylim(0,5)
    ax.set_xlabel('Vertices');ax.grid(axis='y',color='#DDDDDD',lw=.7);ax.set_axisbelow(True)
axes[0].set_ylabel('Median runtime per call (ms)')
fig.legend(*axes[0].get_legend_handles_labels(),loc='upper center',ncol=2,frameon=False,bbox_to_anchor=(.5,1.03))
fig.tight_layout(rect=(0,0,1,.91),w_pad=1.2)
args.output.parent.mkdir(parents=True,exist_ok=True)
fig.savefig(args.output,dpi=300,bbox_inches='tight',facecolor='white')
print(args.output)
if max(float(r['fibonacci_median_of_seed_medians_us']) for r in rows)>5000:
    print('WARNING: a value exceeds the fixed 5 ms axis. Increase the scale before presenting this new run.')
