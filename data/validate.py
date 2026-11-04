#!/usr/bin/env python3
"""Data validation and tiny independent Bellman-Ford oracle; NOT a benchmark."""
import csv
import hashlib
import io
import json
from pathlib import Path
import tempfile
import unittest
import generate

ROOT = Path(__file__).parent

def read_edges(data):
    reader = csv.reader(io.StringIO(data.decode('ascii')))
    assert next(reader) == ['source', 'target', 'weight']
    return [tuple(map(int, row)) for row in reader]

def reachable(n, edges, reverse=False):
    adj = [[] for _ in range(n)]
    for u, v, _ in edges:
        adj[v if reverse else u].append(u if reverse else v)
    seen, stack = {0}, [0]
    while stack:
        for v in adj[stack.pop()]:
            if v not in seen:
                seen.add(v)
                stack.append(v)
    return len(seen)

def reference_distances(n, edges, source):
    # Deliberately simple Bellman-Ford, only for tiny correctness fixtures.
    distances = [None] * n
    distances[source] = 0
    for _ in range(n - 1):
        changed = False
        for u, v, w in edges:
            if distances[u] is not None:
                candidate = distances[u] + w
                if distances[v] is None or candidate < distances[v]:
                    distances[v], changed = candidate, True
        if not changed:
            break
    return distances

class DataTests(unittest.TestCase):
    def test_core_graphs_and_manifest(self):
        manifest = json.loads((ROOT / 'manifest.json').read_text())
        self.assertEqual(manifest['generator_sha256'], generate.sha256((ROOT / 'generate.py').read_bytes()))
        self.assertEqual(len(manifest['graphs']), 33)
        expected = {(n, ratio * n, seed) for n in generate.SIZES for ratio in generate.RATIOS for seed in generate.SEEDS}
        expected |= {(n, n * (n - 1) // 10, seed) for n in (250, 1000) for seed in generate.SEEDS}
        self.assertEqual({(g['n'], g['requested_m'], g['seed']) for g in manifest['graphs']}, expected)
        self.assertEqual({p.name for p in (ROOT / 'graphs').glob('*.csv')}, {Path(g['file']).name for g in manifest['graphs'] if g['family'] == 'core_fixed_average_outdegree'})
        for g in manifest['graphs']:
            with self.subTest(file=g['file']):
                data = (ROOT / g['file']).read_bytes()
                edges = read_edges(data)
                n, m = g['n'], g['requested_m']
                self.assertEqual(len(edges), m)
                self.assertEqual(g['actual_m'], m)
                self.assertEqual(len(data), g['bytes'])
                self.assertEqual(hashlib.sha256(data).hexdigest(), g['sha256'])
                self.assertEqual(len({(u,v) for u,v,_ in edges}), m)
                self.assertTrue(all(0 <= u < n and 0 <= v < n and u != v and 1 <= w <= 1000 for u,v,w in edges))
                self.assertEqual(reachable(n, edges), n)
                self.assertEqual(reachable(n, edges, True), n)
                self.assertEqual(g['requested_density'], 0.10 if g['family'] == 'optional_fixed_density' else m / (n * (n - 1)))
                self.assertEqual(g['actual_density'], m / (n * (n - 1)))
                self.assertEqual(g['sources'], generate.sources(n, g['seed']))
                self.assertEqual(len(set(g['sources'])), 3)
                self.assertTrue(all(0 <= s < n for s in g['sources']))
                self.assertEqual(generate.graph_bytes(n, m, g['seed']), data)
    def test_complete_pack_regeneration(self):
        with tempfile.TemporaryDirectory() as temp:
            generate.write_pack(temp)
            for path in Path(temp).rglob('*'):
                if path.is_file():
                    self.assertEqual(path.read_bytes(), (ROOT / path.relative_to(temp)).read_bytes())
    def test_tiny_correctness_oracle(self):
        fixtures = json.loads((ROOT / 'correctness' / 'expected.json').read_text())['fixtures']
        self.assertEqual(len(fixtures), 4)
        for fixture in fixtures:
            with self.subTest(fixture=fixture['name']):
                data = (ROOT / fixture['file']).read_bytes()
                self.assertEqual(generate.sha256(data), fixture['sha256'])
                self.assertEqual(reference_distances(fixture['n'], read_edges(data), fixture['source']), fixture['expected_distances'])

if __name__ == '__main__':
    unittest.main(verbosity=2)
