
# Section 5.2.4 — Histogram-Based Structural Analysis (ACTIVE+DELETED Maps)

This section analyzes clustering behavior in open-addressed hash tables under an LRU workload, using histogram outputs generated before and after compaction.

## 1. Longest 1-Runs in ACTIVE+DELETED
Before compaction:
- Runs counted: 3  
- Max run length: 3  
- Mean run length: 1.67  

After compaction:
- Runs counted: 1  
- Max run length: 1  
- Mean run length: 1.00  

Interpretation:
- A few medium clusters existed before compaction.
- Compaction eliminated medium clusters entirely, leaving only single-cell runs.

---

## 2. Single vs. Double Probing Clustering
At this N-size, histogram shows:
- Very small run lengths (1–3)
- Minimal clustering overall

This aligns with the elapsed-time plot where single and double probing behave similarly for small N. Differences become more visible as N increases.

---

## 3. Effect of Compaction on Long Runs
Compaction clearly reduces clustering:
- ACTIVE runs collapse from {3,1,1} → {1}
- Bits scanned drop from 8 → 4  
- Tombstone pollution is removed

This reduces probe sequences and improves performance.

---

## 4. Histogram Overlay — Before vs. After
After compaction:
- Long runs shrink or disappear
- Distribution shifts toward short runs (mostly length 1)
- INACTIVE space grows more contiguous (max run length increases to 3)

This is normal free slots merge as occupied slots shrink.

---

## 5. Single vs Double Probing: Long Runs
At this N:
- Both probing strategies yield similar cluster sizes
- Minimal structural divergence

At larger Ns (seen on the elapsed-time curve), single probing eventually forms longer clusters, causing slower performance.

---

## 6. Correlation with average_probes and elapsed_ms
When ACTIVE+DELETED runs grow longer:
- average_probes increases  
- elapsed_ms increases  

 runtime graph reflects this:
- Small N (short runs): lowest elapsed time  
- Large N (longer probe chains): rapid time growth  

Double probing overtakes single probing once clustering becomes severe.

---

## 7. Compaction and Performance Improvement
Compaction:
- Removes tombstones
- Shortens probe paths  
- Reduces effective load factor  
- Improves average_probes and elapsed_ms  

 histogram verifies this via the collapse of runs to length 1.

---

## 8. Relationship to load_factor_pct, eff_load_factor_pct, tombstones_pct
Before compaction:
- effective load > load factor  
- tombstones_pct > 0  
- ACTIVE runs show medium clusters  

After compaction:
- tombstones_pct approaches 0  
- effective load aligns with load factor  
- ACTIVE runs shrink  

This directly matches the structure seen in the histograms.

---

## Final Summary
- table experiences minimal clustering at small N; compaction makes it nearly ideal.
- Single and double probing behave similarly at this N, but diverge at larger N (as seen in the runtime plot).
- Histograms clearly show compaction’s effectiveness in reducing clustering.
- Structural behavior in the maps explains the measured performance patterns.
