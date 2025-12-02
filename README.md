# 📦 Project 5 (Part 2) – Open-Addressed Hash Tables with LRU Workload
**Name:** Zainab Ghafoor  
**ID:** 008259427  
**GitHub repository link:** [[https://github.com/zainabghafoor0/project3_p2.git]((https://github.com/zainabghafoor0/project_5_p2.git))]


This project studies how open-addressed hash tables behave under a **delete-heavy LRU (Least-Recently Used)** workload.

You:

- Reuse a provided `HashTableDictionary` implementation (open addressing with single and double probing).
- Replay LRU profile trace files (`lru_profile_N_..._S_23.trace`) against the table.
- Use a **timing harness** to compare:
  - **Single (linear) probing** vs **double hashing**
  - With compaction turned on
- Export CSV metrics for plotting and analysis (D3 apps).

---

## Directory Layout

Key files in this repo:

- **Core hash table & helpers (given):**
  - `HashTableDictionary.hpp`
  - `HashTableDictionary.cpp`
  - `InvertedListDictionary.hpp`
  - `InvertedListDictionary.cpp`
  - `SmallIntMixedOperations.hpp`
  - `SmallIntMixedOperations.cpp`
  - `Operations.hpp`

- **Standalone driver (given):**
  - `main.cpp`  
    Standalone program that:
    - takes a single LRU trace file,
    - constructs a `HashTableDictionary`,
    - replays the trace once,
    - prints statistics and masks (ACTIVE / DELETED / AVAILABLE, before/after compaction).

- **LRU trace files (given):**
  - `lru_profile_trace_files/`
    - `lru_profile_N_1024_S_23.trace`
    - `lru_profile_N_2048_S_23.trace`
    - `...` (additional N values as provided)

- **Word corpora (for building your own traces later):**
  - `20980712_uniq_words.txt`
  - `50000_words_6770_uniq.txt`
  - `6770_uniq_words.txt`
  - `all_uniq_tokens_imdb_and_newsgroups.txt`

---

## New File Added in This Part

### `LRUHarness.cpp`  ✅ (new)

This is the **timing harness** you added for the LRU profile experiments.

Responsibilities:

- Takes a **trace directory** as a command-line argument (e.g. `lru_profile_trace_files`).
- For each `N` in a predefined list (e.g., 1024, 2048, 4096, …):
  - Constructs the trace path:  
    `lru_profile_N_<N>_S_23.trace`
  - Uses `load_trace_strict_header` to read:
    - `N` and `seed` from the header line
    - operations (`I` / `E`) into a `std::vector<Operation>`
  - Uses `tableSizeForN(N)` to map logical capacity `N` to prime table size `M`.
- For each trace and each probe type:
  - Runs **SINGLE** probing (linear) with compaction enabled.
  - Runs **DOUBLE** probing with compaction enabled.
  - For each configuration:
    - Executes **1 warm-up run** (untimed).
    - Executes **7 timed runs**, measuring elapsed time in milliseconds.
    - Computes the **median elapsed_ms**.
  - Calls `HashTableDictionary::csvStats()` at the end of the last run.
- Writes a **single CSV line per configuration**, with columns:

  ```text
  impl,profile,trace_path,N,seed,elapsed_ms,ops_total,
  <fields from HashTableDictionary::csvStatsHeader()>

