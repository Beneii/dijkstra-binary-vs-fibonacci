# Frozen before performance pilots — 2026-11-01 06:46 UTC

AI-authored hypotheses and protocol, not Ben's prior predictions. Ben explicitly
requested C++ and actual benchmarks. No C++ performance measurements preceded this note.

Hypothesis: indexed binary may win at these sizes due to contiguous storage and simple
sift operations; Fibonacci may improve relatively with more successful decreases.
No crossover is guaranteed. Results will describe this machine and implementation.

Use the exact original ZIP SHA256
23595f5ee9e015af2afc0d0c7404610f51d43d4e2cc42542c588f6c8c9c78345
and all 33 manifest graphs; first listed source for both algorithms. Keep 27 core
constant-average-degree cases separate from six optional fixed-density cases.
Validate hashes and correctness first, including an independent O(n²+m) array oracle.

Both implementations use the same adjacency vectors, Dijkstra template, all-vertex
initialization, strict relaxation, and (distance,vertex ID) ties. Binary code is an
assistant reference, not James's contribution. Fibonacci storage uses RAII node
ownership, so raw ring links never own memory and all nodes release at call end.
Initialization, result allocation and algorithm-owned cleanup are timed; CSV parsing,
result validation and output are excluded. Collect logical counts separately.

MSVC release /O2 /std:c++17 /EHsc /DNDEBUG. steady_clock nanoseconds. Two untimed
warm-up calls per algorithm followed by eight paired trials, alternating order for
four first positions each. For every trial use identical batches: 32 calls at n=250,
8 at n=1000, 2 at n=4000. Record batch elapsed and elapsed/batch, median and inclusive
quartiles/IQR. These counts are fixed before observation, not selected from results.
Eight repetitions measure run variability, not eight independent graphs.

A paired graph timing process has a 120-second wall limit; a timeout is retained
for the pair, never selectively rerun or dropped. Each memory worker has the same
120-second limit. No concurrent benchmark workers. Machine is not reserved/idle-
guaranteed: background apps may add noise. No changes to power or system settings.

Separate fresh Windows worker per graph and algorithm for memory. GetProcessMemoryInfo
reports whole-process peak working set and peak pagefile/private commit, plus baseline
after parsing and before Dijkstra. Peaks include executable/runtime/graph parsing and
allocator history; baseline subtraction is not exact heap bytes and will not be
presented as such. Memory is a single diagnostic run per case, not a distribution.
If this supported Windows API fails, report memory unavailable, not fabricated values.

Limits: supplied weights 1..1000. Parser accepts only exact three-field unsigned
integers and requires weights <= (UINT64_MAX-1)/max(1,n-1), ensuring every simple
shortest path is representable. Non-improving overlarge walks saturate at the infinity
sentinel rather than overflow. Huge integers beyond uint64 are rejected explicitly.
Correctness tests are not timing pilots. Personal reflection, disclosure and group
integration remain students' work; do not attribute AI hypotheses to them.
