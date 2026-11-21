# Layout benchmarks

Ports of a third-party layout benchmark suite
([Goose](https://github.com/aardappel/goose), Apache-2.0, Wouter van
Oortmerssen) — programs chosen for their data shapes, used here to
measure Fid's layouts and to gate proposal
`docs/proposals/0008-contiguous-data.md`. The designs in that proposal
are Fid's own (its Related Work section places them); these ports are
measurements, with each file naming its source.

Ported so far (faithful: the suite's xorshift64 RNG and the same algorithms,
so checksums must match the source suite's rows for the same N):

| Port | Source file | What it measures |
|---|---|---|
| `sum.fd` | the suite's `sum.goose` | The control: flat scalar array, no memory-model advantage — a dead heat is the pass condition |
| `words.fd` | the suite's `words.goose` | Word frequency: the source keys a map by slices into the text; the idiomatic Fid port copies every key into a heap str |
| `sexp.fd` | the suite's `sexp.goose` | The flagship: parse an s-expression tree. Source: one grow-only pool, variant-sized nodes (4..17 bytes), self-relative links. Fid: the index-arena pattern (worst-case 3-word nodes + one heap str per symbol) |

Run:

    python bench/contig/run.py --fid release/fid-dev.exe --n small        # N/20
    python bench/contig/run.py --fid release/fid-dev.exe --n full --native

Times are whole-process medians. On a loaded machine every number is an
upper bound; quote numbers from a quiet machine only (the repo's bench
gates follow the same rule). The runner prints each port's checksum lines —
they must agree across our backends, and with the source suite's at the same N.

The remaining benchmarks of the suite (push, strlist, records, tree, interp,
graph, particles, lru, scene, calc, bintrees, respond, blur) port one per
layout fix that motivates them, as 0008 lands.
