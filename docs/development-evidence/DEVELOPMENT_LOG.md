# Assistance and development evidence — C++

All initial C++ code, tests, orchestration, hypotheses, charts and technical analysis
were produced by the assistant after Ben explicitly requested C++ and actual benchmarks.
No claim is made that Ben wrote them unaided, understood them already, or collaborated
with James on this pack. The reference binary heap is not James's implementation.

Installed compiler discovery found MSVC 2022 Build Tools 14.44.35207. No installation
occurred. CIM hardware lookup was unavailable; the processor name came from the normal
processor registry key and OS from platform metadata. No unrelated user data was read.

Initial C++ compilation succeeded. Selftests and all original graphs/fixtures passed
before timing. The Python review's defects informed C++ regressions: malformed CSV
row widths are rejected, enormous integers reject cleanly, overflow arithmetic is
guarded, and ownership is RAII rather than self-owning cycles. These are preventative
C++ checks, not fabricated C++ failures or student experiences.

The first complete measurement run is retained in results-initial. Code was then
formatted and separated into modules, with CLI bounds/algorithm checks and an explicit
overflow-walk regression added. No heap or Dijkstra optimization was made in response
to results. The final run follows the same predeclared method and is retained in results.
Both raw runs are delivered. Repetitions/trials are not silently selected or pooled.

The Python implementation's genuine initial test/environment failure and independent
review defects remain documented in its separate DEVELOPMENT_LOG.md. Students must
describe their own checking/actions accurately; this log is not a personal reflection.
