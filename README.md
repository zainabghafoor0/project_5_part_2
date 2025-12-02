# 📦 Project 5 (Part 2) – Open-Addressed Hash Tables with LRU Workload
**Name:** Zainab Ghafoor  
**ID:** 008259427  
**GitHub repository:** https://github.com/zainabghafoor0/project_5_p2

This project analyzes the behavior of an **open-addressed hash table** under a **delete-heavy Least-Recently Used (LRU)** workload. The focus is on:

- Understanding probe behavior under churn  
- Comparing **single (linear) probing** vs. **double hashing**  
- Interpreting table structure (ACTIVE, DELETED, AVAILABLE cells)  
- Studying compaction effects  
- Producing reproducible experiments and D3-ready CSV outputs  

You replay provided LRU trace files against a baseline hash-table implementation and build an experimental harness to evaluate performance at multiple table sizes.

---

## 📁 Directory Layout

### **Core hash-table implementation (given)**  
- `HashTableDictionary.hpp`  
- `HashTableDictionary.cpp`  
- `InvertedListDictionary.hpp`  
- `InvertedListDictionary.cpp`  
- `SmallIntMixedOperations.hpp`  
- `SmallIntMixedOperations.cpp`  
- `Operations.hpp`  

### **Standalone driver (given)**  
- `main.cpp`  
  - Loads one LRU trace file  
  - Constructs the hash table  
  - Replays all I/E operations  
  - Prints:  
    - ACTIVE / DELETED / AVAILABLE maps  
    - 0/1 compacted occupancy map  
    - Per-run statistics  
    - Before/after compaction structure  

### **Provided LRU trace files**
Located in:  
`lru_profile_trace_files/`

Examples:  
- `lru_profile_N_1024_S_23.trace`  
- `lru_profile_N_2048_S_23.trace`  
- (More Ns depending on instructor package)

### **Word corpora (for building your own trace generator later)**
- `20980712_uniq_words.txt` (large dataset, ~370MB)  
- `50000_words_6770_uniq.txt`  
- `6770_uniq_words.txt`  
- `all_uniq_tokens_imdb_and_newsgroups.txt`  

These are used only when implementing your custom LRU trace generator at the end of the project.

---

## 🆕 New File Added in This Part

### ✔ `LRUHarness.cpp`  (Timing Harness)  

This file was created for **Part 2**.  
It automates the performance experiments for all Ns across two probe strategies.

### **Responsibilities of `LRUHarness.cpp`:**

#### 1. **Inputs**
- Takes a directory path:  
  ```
  ./HashTableHarness <trace_directory>
  ```
- Expects trace files named:  
  ```
  lru_profile_N_<N>_S_23.trace
  ```

#### 2. **Parsing each trace**
For each N:
- Opens the corresponding trace file  
- Reads the header via `load_trace_strict_header()`:
  - `profile`, `N`, `seed`
- Loads all operations (`I key`, `E key`) into a `std::vector<Operation>`

#### 3. **Table construction**
- Converts logical capacity `N` → prime table size `M`  
  using:
  ```
  tableSizeForN(N)
  ```

#### 4. **Running experiments**
For **each probe type**:

##### ▶ Probe Type 1: `SINGLE` (linear probing)  
##### ▶ Probe Type 2: `DOUBLE` (double hashing)

Each configuration performs:
- **1 warm-up run** (untimed)  
- **7 timed runs**  
- Compute the **median `elapsed_ms`**  

Compaction is **ON** for all runs (default trigger 0.95).

#### 5. **Statistics collection**
At the end of the final replay, the table provides:
- Load factor  
- Effective load factor  
- Tombstones %  
- Total probes  
- Free slots  
- Compactions  
- Maximum occupancy  
- Average probes per op  
- and many more

These come from `HashTableDictionary::csvStats()`.

#### 6. **CSV output format**

Each configuration produces one CSV row:

```
impl,profile,trace_path,N,seed,elapsed_ms,ops_total,
table_size,active,available,tombstones,total_probes,inserts,deletes,lookups,
full_scans,compactions,max_in_table,available_pct,load_factor_pct,
eff_load_factor_pct,tombstones_pct,average_probes,probe_type,compaction_state
```

This CSV is designed for the **D3 timing visualizer** provided with the assignment.
