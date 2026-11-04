"""Matched run of untouched actual implementations. All correctness precedes timing."""
from pathlib import Path
import csv,hashlib,io,json,platform,statistics,subprocess,datetime,argparse

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'runs'/'new-run'
DATA=ROOT/'data'
EXE=ROOT/'build'/'matched.exe'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def call(*args,timeout=120):
    p=subprocess.run([str(EXE),*map(str,args)],cwd=ROOT,text=True,capture_output=True,timeout=timeout)
    if p.returncode:raise RuntimeError(f'{args}: {p.returncode}\n{p.stdout}\n{p.stderr}')
    return p.stdout.strip()
def dump_csv(name,rows):
    with (OUT/name).open('w',newline='',encoding='utf-8')as f:
        writer=csv.DictWriter(f,fieldnames=list(rows[0]));writer.writeheader();writer.writerows(rows)

def main():
    global OUT
    parser=argparse.ArgumentParser(description='Run both submitted implementations; preserve recorded-results.')
    parser.add_argument('--output', type=Path, default=OUT, help='New output directory; existing raw timing results are never overwritten.')
    args=parser.parse_args()
    OUT=args.output.resolve()
    recorded=(ROOT/'results'/'recorded').resolve()
    if OUT==recorded or recorded in OUT.parents:
        raise RuntimeError('recorded-results is preserved; choose a new output directory.')
    if (OUT/'raw-timing.csv').exists():raise RuntimeError('Existing result run preserved: choose another directory before rerunning.')
    OUT.mkdir(parents=True,exist_ok=True)
    start=datetime.datetime.now(datetime.timezone.utc).isoformat()
    manifest=json.loads((DATA/'manifest.json').read_text())
    graphs=[g for g in manifest['graphs']if g['family']=='core_fixed_average_outdegree']
    assert len(graphs)==27
    graphs.sort(key=lambda g:(g['n'],g['actual_m'],g['seed']))
    copies=[]
    for relative, expected in json.loads((ROOT/'docs'/'source-hashes.json').read_text()).items():
        p=ROOT/relative
        assert sha(p)==expected,f'Source changed: {p}'
        copies.append({'copy':relative,'sha256':expected,'matches_recorded_source':True})
    (OUT/'source-provenance.json').write_text(json.dumps(copies,indent=2))
    for g in graphs:assert sha(DATA/g['file'])==g['sha256'],g['file']
    hardware=subprocess.check_output(['powershell','-NoProfile','-Command',"@{cpu=(Get-CimInstance Win32_Processor|Select-Object Name,NumberOfCores,NumberOfLogicalProcessors);os=(Get-CimInstance Win32_OperatingSystem|Select-Object Caption,Version,TotalVisibleMemorySize)}|ConvertTo-Json -Depth 4"],text=True)
    metadata={'started_utc':start,'hardware':json.loads(hardware),'platform':platform.platform(),'compiler':call('compiler'),'flags':'/O2 /std:c++17 /EHsc /DNDEBUG /W4; x64; no LTO','executable_sha256':sha(EXE),'harness_sha256':sha(ROOT/'benchmarks'/'matched.cpp'),'runner_sha256':sha(Path(__file__)),'manifest_sha256':sha(DATA/'manifest.json'),'source_vertex':0,'measured_trials':8,'warmup_calls_per_algorithm':2,'batch_calls_by_vertices':{'250':32,'1000':8,'4000':2},'order':'alternates first algorithm by trial parity; two algorithms measured in same process','timed_scope':'initialization, native algorithm, all algorithm allocations and cleanup, result destruction and one sink update','excluded':'file reading, input parsing, representation conversion, verification, output','comparison_scope':'actual complete implementations, NOT isolated heap-only comparison','implementation_differences':['James inserts discovered vertices; Ben initially inserts all V','Ben returns distances and predecessors; James distances only','James signed long long distances/int weights; Ben uint64 distance/weights','Heap-specific tie-breaking differs'],'measurement_conditions':'Interactive Windows desktop; background load, affinity, power mode and frequency scaling uncontrolled; no deliberate setting changes','memory_measurement':'not measured'}
    checks=[call('selftest')]
    heap=subprocess.run([str(ROOT/'build'/'james-heap-test.exe')],capture_output=True,text=True,check=True)
    assert heap.stdout.split()==['2:','3','0:','5']
    checks.append('PASS James original heap_test: '+heap.stdout.strip().replace('\n','; '))
    fixtures=json.loads((DATA/'correctness/expected.json').read_text())['fixtures']
    for fixture in fixtures:
        assert sha(DATA/fixture['file'])==fixture['sha256']
        d=json.loads(call('distances',DATA/fixture['file'],fixture['n'],fixture['source']))
        assert d==fixture['expected_distances'],fixture['name']
        checks.append('PASS fixture '+fixture['name']+': both full vectors match oracle and published expected distances')
    fields=['status','vertices','source','fib_insertions','fib_extractions','fib_decrease_keys','fib_edges_examined','binary_insertions','binary_extractions','binary_decrease_keys','binary_edges_examined','binary_successful_relaxations']
    counts=[]
    for g in graphs:
        result=call('verify',DATA/g['file'],g['n'],0)
        row=dict(zip(fields,next(csv.reader([result]))));assert row['status']=='PASS'
        row={'dataset':Path(g['file']).name,'graph_sha256':g['sha256'],**row};counts.append(row)
        checks.append('PASS '+Path(g['file']).name+': both full distance vectors vs oracle; Fibonacci predecessor paths verified')
    (OUT/'correctness.txt').write_text('\n'.join(checks)+'\n')
    dump_csv('operation-counts.csv',counts)
    print('ALL CORRECTNESS PASSED: 27 core graphs source0, 4 expected fixtures, 80 random graphs, randomized heaps',flush=True)
    raw=[];summaries=[]
    for g in graphs:
        records=list(csv.DictReader(io.StringIO(call('time',DATA/g['file'],g['n'],0))))
        assert len(records)==16
        for r in records:raw.append({'dataset':Path(g['file']).name,'vertices':g['n'],'edges':g['actual_m'],'seed':g['seed'],'source':0,**r})
        for alg in ['james-binary','ben-fibonacci']:
            series=[float(r['ns_per_call'])for r in records if r['algorithm']==alg];assert len(series)==8
            q1,_,q3=statistics.quantiles(series,n=4,method='inclusive')
            summaries.append({'dataset':Path(g['file']).name,'vertices':g['n'],'edges':g['actual_m'],'seed':g['seed'],'source':0,'algorithm':alg,'median_ns_per_call':statistics.median(series),'median_us_per_call':statistics.median(series)/1000,'q1_ns_per_call':q1,'q3_ns_per_call':q3,'iqr_ns_per_call':q3-q1,'min_ns_per_call':min(series),'max_ns_per_call':max(series),'measured_batches':8,'calls_per_batch':int(records[0]['calls_per_batch']),'graph_sha256':g['sha256']})
        print('TIMED '+Path(g['file']).name,flush=True)
    dump_csv('raw-timing.csv',raw);dump_csv('summary.csv',summaries)
    aggregate=[]
    for n in [250,1000,4000]:
        for ratio in [4,16,64]:
            family={alg:[r['median_us_per_call']for r in summaries if r['vertices']==n and r['edges']==n*ratio and r['algorithm']==alg]for alg in ['james-binary','ben-fibonacci']}
            binary=statistics.median(family['james-binary']);fib=statistics.median(family['ben-fibonacci'])
            aggregate.append({'vertices':n,'edge_to_vertex_ratio':ratio,'edges':n*ratio,'seed_count':3,'binary_median_of_seed_medians_us':binary,'fibonacci_median_of_seed_medians_us':fib,'fibonacci_over_binary_ratio_of_aggregates':fib/binary})
    dump_csv('aggregate.csv',aggregate)
    metadata['finished_utc']=datetime.datetime.now(datetime.timezone.utc).isoformat();metadata['correctness']='PASS';metadata['timed_rows']=len(raw);metadata['summary_rows']=len(summaries)
    (OUT/'environment.json').write_text(json.dumps(metadata,indent=2))
    print(json.dumps(aggregate,indent=2))

if __name__=='__main__':main()
