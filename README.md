# Dijkstra algorithm comparison

By Benjamin Jones and James Novosel, 41052 Advanced Algorithms.

One repository containing both submitted implementations and the shared experiment.

## Assessment artifacts

- [Group report](report/Group-Report.pdf)
- [Presentation video](presentation/Presentation.mp4) (11:07; use Download raw file if GitHub does not play it)
- [Benjamin Jones individual reflection](reflections/Ben-Jones-Individual-Reflection.pdf)
- [James Novosel individual reflection](reflections/James-Novosel-Individual-Reflection.docx)
- [Presentation slides](slides/Presentation-Slides.pdf)
- [Video methodology clarification](presentation/VIDEO-CLARIFICATION.txt)

The source code and reproduction instructions are below. AI use is disclosed in the report and module documentation. The report explicitly identifies the evidence limitation concerning student-authored pre-test predictions.

## Repository layout

```text
src/
  binary-heap/       James Novosel's binary heap and Dijkstra
  fibonacci-heap/    Ben Jones's Fibonacci heap and Dijkstra
benchmarks/
  matched.cpp        Shared verification and paired timing harness
tests/
  binary_heap_test.cpp  James's original standalone heap test
scripts/
  run_benchmarks.py  Run correctness checks, then collect measurements
  audit_results.py  Check statistics and source/data hashes
  plot_results.py   Regenerate the comparison chart
data/               Shared graphs, manifest, fixtures and generator
results/
  recorded/         Original measurements used in the report
figures/            Runtime chart
docs/               Validation, source hashes and development evidence
build.py            Build both implementations and the test
requirements.txt    Optional charting dependency
```

These are algorithm modules, not separate repositories. James is responsible for binary and Ben for Fibonacci. Extensive AI assistance, including generated code, is disclosed in the report and module READMEs. Primary-author responsibility does not imply unaided authorship.

## Build and run

Requirements: Windows 10/11, Python 3.10 or newer, Visual Studio C++ Build Tools with the x64 compiler and Windows SDK. Open a terminal in this repository folder:

```powershell
python build.py
python scripts/audit_results.py
python scripts/run_benchmarks.py --output runs/reproduction
python scripts/audit_results.py --results runs/reproduction
```

The build discovers Visual Studio through vswhere, or uses a configured x64 Native Tools prompt. Outputs go into build/, excluded from the submission. The preserved harness uses MSVC macros; Linux/macOS and other compilers have not been tested. Run from an extracted folder, not a ZIP viewer.

To regenerate the report chart:

```powershell
python -m pip install -r requirements.txt
python scripts/plot_results.py
```

This reads results/recorded/aggregate.csv and writes figures/runtime-comparison.png. Use --results runs/reproduction --output runs/reproduction/runtime.png to plot a new run. The chart uses a common 0-5 ms scale; check the warning and expand the scale if fresh measurements exceed it.

## Preserved results and reproduction

results/recorded/ contains the original 4 November measurements: 432 raw batches, 54 instance summaries, nine aggregates, correctness and environment records. New runs never overwrite these files. A new output directory is required once raw timing data exists. Different machines, compilers and background activity will change timings; identical timings or rankings are not guaranteed.

Before timing, the runner checks all 27 core inputs, four expected-distance fixtures, 80 random shortest-path cases and 18,000 randomized operations for each heap. It also runs James's original heap test. Source is 0; each input receives two warm-ups and eight paired batches with alternating execution order. Parsing/conversion are excluded; initialization, algorithm work and cleanup are included.

docs/source-hashes.json pins the algorithm/test source bytes to the measured copies. Historical original-source paths in the saved metadata are evidence only; portable scripts do not open them. The audit translates old module names to their new paths. Only harness include paths and support/build scripts changed during organisation; algorithm bodies and recorded result files are unchanged. Historical executable hashes are retained but not asserted against new executables. A new-run audit does check its executable identity.

## Experimental scope

The 27 synthetic directed graphs use V = 250, 1000, 4000; E/V = 4, 16, 64; seeds 11, 22, 33. All tested families are asymptotically sparse. Optional density inputs in data/ were not part of the final matched run. The data README explains generation, connectivity bias and validation.

This compares complete implementations, not isolated heap choice. James inserts discovered vertices, while Ben initially queues all vertices and returns predecessors. Integer types, ties and counter semantics differ. Memory and cache costs were not measured. The AI-generated reference binary header in the Fibonacci module is an unused benchmark dependency, not James's implementation; it is not instantiated or timed by the shared harness.

Both actual implementations are included here. There is no second repository or nested repository ZIP to locate.
