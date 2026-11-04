from pathlib import Path
import csv,hashlib,json,statistics,math,argparse
ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description='Recompute saved statistics and verify supplied source/data hashes.')
parser.add_argument('--results',type=Path,default=ROOT/'results'/'recorded')
OUT=parser.parse_args().results.resolve()
def rows(name):return list(csv.DictReader((OUT/name).open(newline='')))
raw=rows('raw-timing.csv');summary=rows('summary.csv');aggregate=rows('aggregate.csv')
assert len(raw)==432 and len(summary)==54 and len(aggregate)==9
for s in summary:
    r=[x for x in raw if x['dataset']==s['dataset']and x['algorithm']==s['algorithm']]
    assert len(r)==8 and {int(x['trial'])for x in r}==set(range(8))
    assert all(float(x['ns_per_call'])>0 for x in r)
    values=[float(x['ns_per_call'])for x in r]
    q1,_,q3=statistics.quantiles(values,n=4,method='inclusive')
    assert math.isclose(statistics.median(values),float(s['median_ns_per_call']),abs_tol=1e-5)
    assert math.isclose(q3-q1,float(s['iqr_ns_per_call']),abs_tol=1e-5)
for a in aggregate:
    for alg,col in [('james-binary','binary_median_of_seed_medians_us'),('ben-fibonacci','fibonacci_median_of_seed_medians_us')]:
        vals=[float(s['median_us_per_call'])for s in summary if s['vertices']==a['vertices']and s['edges']==a['edges']and s['algorithm']==alg]
        assert len(vals)==3 and math.isclose(statistics.median(vals),float(a[col]),abs_tol=1e-9)
def source_path(old):
    old=old.replace('\\','/')
    if old=='james/heap_test.cpp':return 'tests/binary_heap_test.cpp'
    return old.replace('ben/','src/fibonacci-heap/',1).replace('james/','src/binary-heap/',1)

for p in json.loads((OUT/'source-provenance.json').read_text()):
    p={**p,'copy':source_path(p['copy'])}
    current=hashlib.sha256((ROOT/p['copy']).read_bytes()).hexdigest()
    expected=json.loads((ROOT/'docs'/'source-hashes.json').read_text())[p['copy']]
    assert current==expected==p['sha256'],p['copy']
meta=json.loads((OUT/'environment.json').read_text())
if OUT!=(ROOT/'results'/'recorded').resolve():
    assert hashlib.sha256((ROOT/'build/matched.exe').read_bytes()).hexdigest()==meta['executable_sha256']
manifest=json.loads((ROOT/'data/manifest.json').read_text())
for g in manifest['graphs']:
    if g['family']=='core_fixed_average_outdegree':assert hashlib.sha256((ROOT/'data'/g['file']).read_bytes()).hexdigest()==g['sha256']
print('PASS: 432 raw rows,54 summaries,9 aggregates; exact trial coverage; medians/IQR recomputed; packaged source hashes and all27graph hashes. Original executable is intentionally not distributed.')
