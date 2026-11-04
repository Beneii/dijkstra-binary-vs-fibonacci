# Factual assistance log — 1 November 2026

The assistant wrote all initial code, tests, harness and technical notes in this pack.
Ben stated that he had not started. This does not establish that he personally wrote,
debugged or understood the implementation, or that James collaborated on this pack.

Observed development issue: the assistant initially used the default TemporaryDirectory
location in its CSV test. On this managed Windows environment the generated sandbox
temporary directory rejected fixture writes and cleanup with PermissionError. Five
test methods passed and one errored. The assistant changed TemporaryDirectory to use
the project directory; the subsequent run passed all six methods. This is one genuine
environment-assumption/testing failure, not three invented algorithm bugs. The group
must decide how to discuss it accurately and record other genuine examples as they occur.

Dataset access: Library prepare_materialize resolved the exact known file to an
authorized signed URL, size 7,670,406 bytes. Invoke-WebRequest failed: 'No such host
is known.' No dataset bytes or checksum were verified, and no full-suite correctness
claim is made. Follow-on hash/extract commands reported the missing local ZIP.

Final correctness run: Python 3.12.10, six unittest methods, all passed. CLI fixture
distances [0,2,0,2,null]; operations inserts=5, extracts=5, decreases=5, scanned_arcs=6.
Benchmark --help succeeded. Performance and memory modes have not been executed.

No personal reflections, prior predictions, surprises, collaboration claims, or
benchmark results were authored on a student's behalf. METHOD_2026-11-01.md is explicitly
assistant-authored. No course submissions or outbound messages were sent.

Recovery: the follow-up checked Downloads and found the exact existing original
dijkstra-synthetic-data-v1.zip (7,670,406 bytes). SHA256 matched the expected
23595f5ee9e015af2afc0d0c7404610f51d43d4e2cc42542c588f6c8c9c78345.
No new download was necessary. validate_dataset.py passed all 33 manifest graphs at
their first-listed source and all four supplied correctness fixtures: per-file hashes,
byte/edge counts, independent array-oracle distances, equal heap outputs/operation
counts, and predecessor edge sums/acyclic chains. This is not a performance pilot.

Second genuine issue found by assistant review: the first benchmark harness allowed
automatic garbage collection during trials, although Fibonacci node rings retain
cycles. Deferred collection could distort or shift costs between algorithms. Fixed
before any performance run by collecting outside each interval and disabling auto-GC
for both algorithms during each timed call. This excludes deferred cyclic cleanup;
that limitation is now disclosed in the method. This is a potential methodology flaw
found by inspection, not an observed timing anomaly. No third failure is invented.

Recheck commands: python -m unittest -v (six methods PASS); python -m py_compile
benchmark.py validate_dataset.py (exit 0); python validate_dataset.py
..\data\dijkstra-synthetic-data-v1 (33 graphs + four fixtures PASS, exit 0).

Independent review identified two further genuine defects: DictReader accepted an
extra fourth column silently, and math.isinf converted a huge accepted integer to
float, raising OverflowError. Fixed row-width rejection (extra and missing fields),
and changed infinity checks in algorithm/oracle/CLI to exact sentinel comparison.
Regression tests use 10**310 weights and a path totaling 2*10**310; both heaps and
the oracle agree. No supplied dataset result had depended on these edge cases.

The reviewer also demonstrated retired Fibonacci self-cycles surviving until GC.
Earlier version 1 already had GC timing isolation; this revision additionally clears
retired links/ownership and explicitly clears any unreachable live tail in both heap
interfaces. A weakref regression confirms immediate release without cyclic GC for
repeated connected and disconnected calls. Eight unittest methods pass. All changes
are assistant-authored; no personal student debugging experience is claimed.
