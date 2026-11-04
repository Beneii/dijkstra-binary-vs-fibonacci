#!/usr/bin/env python3
"""Generate data only; no priority queues or benchmark implementations."""
import argparse
import csv
import hashlib
import io
import json
import random
from pathlib import Path

SIZES = (250, 1000, 4000)
RATIOS = (4, 16, 64)
SEEDS = (11, 22, 33)

def graph_bytes(n, m, seed):
    if not n >= 2 or not n <= m <= n * (n - 1):
        raise ValueError('Require 2 <= n and n <= m <= n*(n-1)')
    rng = random.Random(seed)
    cycle = list(range(n))
    rng.shuffle(cycle)
    arcs = {(cycle[i], cycle[(i + 1) % n]) for i in range(n)}
    while len(arcs) < m:
        u, v = rng.randrange(n), rng.randrange(n)
        if u != v:
            arcs.add((u, v))
    # Sorting before drawing weights removes hash/set-order dependence.
    edges = [(u, v, rng.randint(1, 1000)) for u, v in sorted(arcs)]
    return csv_bytes(edges)

def csv_bytes(edges):
    out = io.StringIO(newline='')
    writer = csv.writer(out, lineterminator='\n')
    writer.writerow(('source', 'target', 'weight'))
    writer.writerows(edges)
    return out.getvalue().encode('ascii')

def sources(n, seed):
    return random.Random(1000003 + seed).sample(range(n), 3)

def sha256(data):
    return hashlib.sha256(data).hexdigest()

def write_pack(root):
    root = Path(root)
    (root / 'graphs').mkdir(parents=True, exist_ok=True)
    manifest = {'format_version': 1, 'kind': 'generated synthetic data, not observed real-world data',
                'generator': 'generate.py', 'generator_sha256': sha256(Path(__file__).read_bytes()),
                'vertex_ids': '0..n-1', 'directed': True, 'weight_range': [1, 1000],
                'density_definition': 'm / (n * (n - 1))', 'graphs': []}
    for n in SIZES:
        for ratio in RATIOS:
            m = ratio * n
            for seed in SEEDS:
                name = f'graphs/n{n}_m{m}_seed{seed}.csv'
                data = graph_bytes(n, m, seed)
                (root / name).write_bytes(data)
                manifest['graphs'].append({'file': name, 'n': n, 'requested_m': m, 'actual_m': m,
                    'm_over_n': ratio, 'requested_density': m / (n * (n - 1)),
                    'actual_density': m / (n * (n - 1)), 'seed': seed,
                    'sources': sources(n, seed), 'bytes': len(data), 'sha256': sha256(data)})
    for g in manifest['graphs']:
        g['family'] = 'core_fixed_average_outdegree'
    (root / 'optional_fixed_density').mkdir(exist_ok=True)
    for n in (250, 1000):
        m = n * (n - 1) // 10
        for seed in SEEDS:
            name = f'optional_fixed_density/n{n}_p0p10_m{m}_seed{seed}.csv'
            data = graph_bytes(n, m, seed)
            (root / name).write_bytes(data)
            manifest['graphs'].append({'file': name, 'family': 'optional_fixed_density', 'n': n,
                'requested_m': m, 'actual_m': m, 'm_over_n': m / n,
                'requested_density': 0.10, 'actual_density': m / (n * (n - 1)),
                'seed': seed, 'sources': sources(n, seed), 'bytes': len(data), 'sha256': sha256(data)})
    (root / 'manifest.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    fixtures = [
        {'name': 'positive_hand_check', 'n': 5, 'source': 0,
         'edges': [(0,1,2),(0,2,8),(1,2,3),(1,3,7),(2,3,1),(3,4,4),(4,0,9)],
         'expected_distances': [0,2,5,6,10]},
        {'name': 'unreachable', 'n': 4, 'source': 0,
         'edges': [(0,1,2),(2,3,1)], 'expected_distances': [0,2,None,None]},
        {'name': 'zero_weight', 'n': 4, 'source': 0,
         'edges': [(0,1,0),(0,2,5),(1,2,0),(2,3,2)], 'expected_distances': [0,0,0,2]},
        {'name': 'equal_distance_tie', 'n': 4, 'source': 0,
         'edges': [(0,1,1),(0,2,1),(1,3,1),(2,3,1)], 'expected_distances': [0,1,1,2]},
    ]
    (root / 'correctness').mkdir(exist_ok=True)
    for fixture in fixtures:
        data = csv_bytes(fixture.pop('edges'))
        fixture['file'] = f"correctness/{fixture['name']}.csv"
        fixture['sha256'] = sha256(data)
        (root / fixture['file']).write_bytes(data)
    (root / 'correctness' / 'expected.json').write_text(json.dumps({
        'purpose': 'Correctness only, not benchmark workloads. null means unreachable.',
        'fixtures': fixtures}, indent=2) + '\n', encoding='utf-8')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--output', type=Path, default=Path(__file__).parent)
    write_pack(parser.parse_args().output)
