# Submission packaging validation

4 November 2026. The packaged code was built from the new submission directory using MSVC x64. The source module hashes match the original measured source copies.

A fresh full run completed successfully using the portable runner: 27 core inputs, four expected-distance fixtures, 80 random shortest-path checks and 18,000 randomized operations for each actual heap. James's original heap test also passed. New timing data was written outside this submission folder to a separate validation directory; no recorded report measurements were overwritten or replaced.

The saved report dataset passed an independent recomputation of medians, IQRs, aggregates, trial coverage and graph/source hashes. The fresh-run dataset was also audited. The portable build and runner are Windows/MSVC specific; no other operating system or compiler was tested.

Compiled outputs are excluded from the distributable repository. Run build.py before starting new experiments. docs/source-hashes.json is checked in place of opening historical original-source paths. The historical executable hash is retained as metadata, not asserted against a new executable.

After the final directory reorganisation, the build, full benchmark/correctness run, new-result audit, original-result audit and chart generation all passed again. The final source directories use algorithm names; tests and scripts are separated. No nested repository archive is supplied.
