# Matched comparison of Ben's and James's actual implementations

Recorded 4 November 2026. This is a separate integration workspace. Neither source
repository was merged, edited or published. Source copies and original paths/hashes
are recorded in `results/source-provenance.json`.

## What was compared

- `james/`: downloaded `C:/Users/Admin/Downloads/Dijkstra's Algorithm/Dijkstra's Algorithm`.
  Actual `BinaryMinHeap` and `dijkstraBinary`, not the assistant-reference binary heap.
- `ben/`: `C:/Users/Admin/Desktop/Ben-Algorithms-A2-2026-11-01/AlgorithmsA2-Cpp`.
  Actual `Fibonacci` and its original `dijkstra<Fibonacci>` template.
- `matched.cpp`: new integration and measurement harness. A preprocessor alias gives
  Ben's `Graph` the name `BenGraph` to coexist with James's `Graph`. No algorithm body
  was changed. The original `reference_binary.hpp` remains an untouched dependency
  of Ben's original header, but is never instantiated or timed by this harness.

The comparison is of the complete implementations, not a causal isolation of heap
choice. James inserts a vertex on discovery; Ben inserts all vertices initially.
Ben also produces predecessors and uses unsigned 64-bit edge weights; James returns
distances only, with signed 64-bit distances and 32-bit weights. Their tie-breaking
policies differ. Input weights and safe path bounds were checked for both domains.

## Build and reproduce

From this directory in PowerShell:

```powershell
.\build.cmd
python run_matched.py
python audit_results.py
```

The recorded run is preserved: `run_matched.py` refuses to overwrite an existing
`results/raw-timing.csv`. For a new run, copy this whole integration workspace to a
new directory and preserve/remove only its copied result files deliberately.
The build needs the existing Visual Studio 2022 Build Tools x64 installation.
Exact commands in `build.cmd` compile both actual implementations into one executable:

```text
cl /nologo /O2 /std:c++17 /EHsc /DNDEBUG /W4 matched.cpp james\dijkstra_binary.cpp /Fe:matched.exe
```

No software was installed. Python standard library performs orchestration and analysis.
The executable and all copied inputs/source files are hashed in the result records.

## Correctness before timing

All correctness checks completed before the first recorded measurement:

- 27 supplied core graphs at **source vertex 0**, with full distance vectors from both
  algorithms checked against an independent array-based Dijkstra oracle.
- Four published fixtures (positive hand check, unreachable vertices, zero-weight
  edges and equal-distance ties), including equality to their published expected
  vectors. Ben's predecessor paths were also checked.
- 80 additional fixed-seed random graphs containing ties, zero weights and disconnected
  cases. Both complete vectors matched the oracle.
- 18,000 fixed-seed randomized heap operations for each actual heap. James's position
  membership and extracted minimum distances were checked against an independent
  model; Ben's heap invariant checker was called after every operation.
- James's original `heap_test.cpp` was built and run unchanged. Its expected output
  was `2: 3` followed by `0: 5`.

This does not establish correctness for arbitrary invalid or overflowing inputs.
The harness rejects inputs beyond the safe shared integer domain instead of changing
James's arithmetic. Original source copies still hash identically after the run.

## Matched measurement protocol

- Same Windows 11 Home 10.0.26200 machine: AMD Ryzen 5 7500F, 6 cores/12 threads,
  approximately 32 GB installed RAM. Full runtime metadata is in `environment.json`.
- Same x64 MSVC 194435223 compiler, C++17, `/O2 /DNDEBUG`, no LTO, single-thread algorithms.
- Same 27 core graph files, hashes checked against the supplied manifest. Sizes 250,
  1,000 and 4,000; E/V ratios 4, 16 and 64; seeds 11, 22 and 33. Source is explicitly
  0 for this run, rather than the manifest's suggested source list.
- One shared strict parser loads the graph and builds each native adjacency
  representation **before** the clock starts. Both representations remain resident.
- Two untimed warm-up calls per implementation, then eight paired trials per graph.
  First/second measurement order alternates on each trial.
- Each timed batch contains 32, 8 or 2 calls for V=250, 1,000 or 4,000 respectively.
  Per-call time is batch duration divided by the number of calls.
- Time includes native initialization, algorithm allocations, shortest-path computation,
  heap/temporary cleanup, returned-result destruction and one anti-optimization sink
  update. File reading, parsing, graph conversion, verification and output are excluded.
- Per-instance result is the median of eight batch means. Inclusive Q1, Q3 and IQR are
  retained. Each aggregate plot point is the median of three seed-specific medians.

This is an interactive desktop run. Background load, CPU affinity, power mode and
frequency scaling were uncontrolled. No memory benchmark was performed. Short batches
and only three graph seeds limit precision and generalisation. No result was selected
from alternative reruns, and no algorithm tuning followed these measurements.

## Recorded result

Both full distance vectors passed on all 27 graphs. James's implementation had the
lower median runtime on **all 27 instances**. No crossover occurred in this dataset.
The per-instance Fibonacci/binary ratio ranged from **3.289 to 18.919**, with median
**9.754**. These ratios describe these complete implementations on this machine.

The table reports microseconds and medians of the three seed-specific medians.
The last column is the ratio of those aggregate medians, not a pooled-trial speedup.

| V | E/V | James binary (µs) | Ben Fibonacci (µs) | Fibonacci / binary |
|---:|---:|---:|---:|---:|
| 250 | 4 | 9.092 | 157.444 | 17.316 |
| 250 | 16 | 15.272 | 184.298 | 12.068 |
| 250 | 64 | 31.073 | 184.466 | 5.936 |
| 1,000 | 4 | 53.769 | 855.575 | 15.912 |
| 1,000 | 16 | 86.950 | 907.825 | 10.441 |
| 1,000 | 64 | 263.719 | 926.694 | 3.514 |
| 4,000 | 4 | 421.775 | 4,113.850 | 9.754 |
| 4,000 | 16 | 705.425 | 4,329.750 | 6.138 |
| 4,000 | 64 | 1,331.150 | 4,438.200 | 3.334 |

Within every tested vertex size, the aggregate ratio narrows as E/V increases, but
does not reverse. This is a descriptive pattern, not proof of its cause. Initialization,
allocation, representation, predecessor production and tie-breaking differ. The run
does not disprove Fibonacci's amortised asymptotic decrease-key bound, predict a future
crossover, or rank every binary/Fibonacci implementation.

Operation counts also use different semantics: Ben decreases a key on every successful
relaxation after initially inserting all vertices; James inserts first discoveries and
counts only later improvements as decrease-key. Do not compare those counters directly
without explaining this distinction.

## Result files

- `results/raw-timing.csv`: all **432** batches (27 graphs × 8 trials × 2 implementations).
- `results/summary.csv`: **54** per-instance algorithm summaries with inclusive quartiles.
- `results/aggregate.csv`: **9** graph-family aggregates, in microseconds.
- `results/operation-counts.csv`: untimed counters from the actual algorithms.
- `results/correctness.txt`: randomized checks, original James test and graph/fixture passes.
- `results/environment.json`: machine, build, timing scope and run identity.
- `results/source-provenance.json`: original locations and identical-copy SHA-256 hashes.
- `results/audit.txt`: independent recomputation and preservation checks.
