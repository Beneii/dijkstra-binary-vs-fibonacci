# Shared synthetic graph data — Ben + James

## Status and scope
This is **generated synthetic data**, not a downloaded real-world dataset. Neither Dijkstra implementation nor performance measurements are included. The small Bellman–Ford routine in `validate.py` is an independent correctness oracle for four tiny fixtures only; it is not an implementation to benchmark. Pilot runtime and memory costs are unknown.

## Frozen common input contract
- Default core: 27 benchmark CSVs: n = 250, 1,000, 4,000; m = 4n, 16n, 64n; seeds = 11, 22, 33.
- Optional separate fixed-density family: 6 CSVs in `optional_fixed_density/`, n = 250 or 1,000, requested density p = 0.10, m = floor(0.10 × n × (n−1)), seeds = 11, 22, 33. Filenames are `n{n}_p0p10_m{m}_seed{seed}.csv`; edge counts are 6,225 and 99,900 respectively. This family is optional, not part of the default 27. Manifest `family` distinguishes the two.
- Directed weighted graphs. Vertex IDs are zero-based, 0 through n−1. CSV header is `source,target,weight`; ASCII encoding, LF line endings, sorted arcs.
- Exactly m distinct arcs, no self-loops or parallel arcs. Positive integer weights are uniform draws from 1 through 1,000. Weights are arbitrary synthetic costs, **not** travel times, distances, or measured costs.
- A shuffled directed Hamiltonian cycle is planted first. Remaining ordered pairs are sampled uniformly with rejection of duplicates/self-loops. This guarantees strong connectivity but **biases the graph distribution** toward graphs containing the planted cycle; this is not an unbiased sample of all connected graphs. Weights are drawn after sorting arcs.
- `manifest.json` records each filename, n, requested/actual m, requested/actual directed density m/[n(n−1)], seed, three distinct shared source IDs, byte count, and SHA-256. A separate seeded RNG chooses sources. Same-n/seed graphs reuse the same source choices across density settings.
- Both algorithms must consume **the same file bytes and the same predeclared source subset in the same order**. The minimum protocol uses the first listed manifest source for every selected graph. If pilot feasibility permits, expand to all three listed sources symmetrically across selected conditions; decide this before collecting comparative results, never after seeing which algorithm wins. The manifest supplies three predetermined distinct source IDs per graph. Do not independently regenerate “similar” graphs or assign new weights. The delivered bytes and checksums are the source of truth. Seeds and the generator allow auditing/regeneration; Python random helper behavior is not promised identical across every future Python version.

The ratios 4/16/64 are average outdegrees and vary density at each fixed n, but all three families have m proportional to n. Actual density falls as n rises. The core **does not test asymptotically dense m=Θ(n²) scaling**. The optional p=0.10 family follows a fixed-fractional-density m=Θ(n²) construction, though two modest sizes alone cannot establish asymptotic behavior. At n=250, the 64n condition has density about 25.7%; at n=4,000 it is about 1.60%.

## Reproduce and validate (Python standard library only)
```sh
python3 generate.py --output .
python3 validate.py
```
Run from this folder. To preserve the frozen files, generate into a separate directory (`--output /tmp/dijkstra-regenerated`) instead; `validate.py` validates the pack beside its script. Tests fully regenerate the pack in a temporary directory and check byte equality, manifest integrity, exact counts, bounds, duplicate/self-loop absence, positive weights, reachability in both orientations (strong connectivity), density/source consistency, and tiny-fixture expected distances.

`correctness/` is **not** benchmark input. Its fixtures cover a hand-checkable positive graph, unreachable vertices, zero weights, and equal-distance ties. `expected.json` supplies n, source, expected distances, and CSV checksums; `null` means unreachable. Zero weights are valid for Dijkstra but intentionally excluded from the core positive-weight suite. When ties occur, compare distances, not a unique predecessor tree.

## Coordination before experiments
1. Ben and James agree on language/runtime, graph representation, numeric type, heap API and decrease-key semantics. A true indexed binary heap versus Fibonacci heap is a different comparison from a lazy duplicate-entry binary heap; state which is implemented.
2. Freeze correctness behavior first: all vertices represented, nonnegative weights only, unreachable distance convention, and matching distance arrays for both algorithms.
3. **Background theory, not your recorded prediction:** Ben and James must write and date their own theory predictions **before** running benchmarks. With adjacency lists and proper decrease-key, typical bounds are binary heap O((n+m) log n) versus Fibonacci heap O(m+n log n) amortized. On m=Θ(n) workloads both are O(n log n); at larger m the Fibonacci bound suggests an advantage, but constants, pointer chasing and allocation may dominate these sizes. These are example hypotheses to consider, not the authors’ recorded predictions or measured results.
4. **Background theory, not your recorded prediction:** write and date your own memory hypothesis before measuring. An example hypothesis is: both need graph storage Θ(n+m), plus per-vertex/heap state, while Fibonacci-heap nodes may have higher constant overhead. Distinguish total process peak memory from extra algorithm memory and state what the measurement includes. Lazy duplicate heaps have different storage behavior.
5. Use identical hardware/software, input files, sources, representation, stopping rule (full single-source shortest paths), warm-up policy and repetition counts. Keep graph loading/parsing/generation outside algorithm timing, or report those separately. Report each source/seed consistently; three sources on one graph are not three independent graph samples. Start with a pilot and report measured feasibility before adding larger workloads.

## Generation environment
Generated and validated with CPython 3.12.14 using only the standard library. `validation-output.txt` records data-check outcomes; its elapsed test duration is **not** a Dijkstra benchmark result.

## AI disclosure
This data pack (generator, synthetic files, README, manifest and validation) was prepared with OpenAI assistant assistance at the user's request. No algorithm performance result has been generated or inferred from these files. Ben and James should review and disclose AI assistance as required by their course policy and write their own pre-experiment predictions before collecting results.
