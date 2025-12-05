// LRUHarness.cpp
// Timing harness for LRU profile traces.
// Uses provided HashTableDictionary and Operations.hpp
// to replay lru_profile_N_*_S_23.trace files and emit CSV lines.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>

#include "HashTableDictionary.hpp"
#include "Operations.hpp"

// ---------- Trace loader (copied from main.cpp) ----------
// The first line of the header must contain:  <profile> <N> <seed>
// After the header: blank lines and lines starting with '#' are ignored.
// Opcodes: I <word1> <word2>  | E <word1> <word2>

bool load_trace_strict_header(const std::string &path,
                              std::size_t &N,
                              std::vector<Operation> &out_operations) {
    std::string profile = "";
    int seed = 0;
    out_operations.clear();

    std::ifstream in(path);
    if (!in.is_open())
        return false;

    // --- read FIRST line as header
    std::string header;
    if (!std::getline(in, header))
        return false;

    // Look for a non-white-space character
    const auto first = header.find_first_not_of(" \t\r\n");
    // Since this is the first line, we don't expect it to be blank
    // or start with a comment.
    if (first == std::string::npos || header[first] == '#')
        return false;

    // Create a string stream so that we can read the profile name,
    // N, and the seed more easily.
    std::istringstream hdr(header);
    if (!(hdr >> profile >> N >> seed))
        return false;

    // --- read ops, allowing comments/blank lines AFTER the header ---
    std::string line;
    while (std::getline(in, line)) {
        const auto opCodeIdx = line.find_first_not_of(" \t\r\n");
        if (opCodeIdx == std::string::npos || line[opCodeIdx] == '#')
            continue; // skip blank and comment lines.

        std::istringstream iss(line.substr(opCodeIdx));
        std::string tok;
        if (!(iss >> tok))
            continue;

        // In the provided traces, each key is stored as two tokens,
        // which we join with a space to form the full string key.
        std::string w1, w2;
        if (tok == "I") {
            if (!(iss >> w1 >> w2)) return false;
            out_operations.emplace_back(OpCode::Insert, w1.append(" ") + w2);
        } else if (tok == "E") {
            if (!(iss >> w1 >> w2)) return false;
            out_operations.emplace_back(OpCode::Erase, w1.append(" ") + w2);
        } else {
            std::cout << "Unknown operation in load_trace_strict_header: "
                      << tok << std::endl;
            return false; // unknown token
        }
    }

    return true;
}

// ---------- Mapping from N to table size M (copied from main.cpp) ----------

std::size_t tableSizeForN(std::size_t N) {
    static const std::vector<std::pair<std::size_t, std::size_t>> N_and_primes = {
        /* N = 2^10  = 1,024      */ { 1024,     1279    },
        /* N = 2^11  = 2,048      */ { 2048,     2551    },
        /* N = 2^12  = 4,096      */ { 4096,     5101    },
        /* N = 2^13  = 8,192      */ { 8192,    10273   },
        /* N = 2^14  = 16,384     */ { 16384,   20479   },
        /* N = 2^15  = 32,768     */ { 32768,   40849   },
        /* N = 2^16  = 65,536     */ { 65536,   81931   },
        /* N = 2^17  = 131,072    */ { 131072,  163861  },
        /* N = 2^18  = 262,144    */ { 262144,  327739  },
        /* N = 2^19  = 524,288    */ { 524288,  655243  },
        /* N = 2^20  = 1,048,576  */ { 1048576, 1310809 }
    };

    for (auto item : N_and_primes) {
        if (item.first == N)
            return item.second;
    }
    std::cout << "Unable to find table size for " << N
              << " in tableSizeForN." << std::endl;
    std::exit(1);
}

// ---------- Small helpers ----------

double median_of(std::vector<double> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

void replay_ops(HashTableDictionary &table,
                const std::vector<Operation> &ops) {
    for (const auto &op : ops) {
        switch (op.tag) {
            case OpCode::Insert:
                table.insert(op.key);
                break;
            case OpCode::Erase:
                (void)table.remove(op.key);
                break;
        }
    }
}

// Run one condition (probeType & compaction) on one trace,
// produce ONE CSV line.
void run_condition(const std::string &tracePath,
                   HashTableDictionary::PROBE_TYPE probeType,
                   bool doCompact) {
    std::size_t N = 0;
    std::vector<Operation> ops;

    if (!load_trace_strict_header(tracePath, N, ops)) {
        std::cerr << "Failed to load trace " << tracePath << "\n";
        return;
    }

    std::size_t M = tableSizeForN(N);
    const int NUM_TIMED_RUNS = 7;

    // --- warm-up (untimed) ---
    {
        HashTableDictionary table(M, probeType, doCompact);
        replay_ops(table, ops);
    }

    std::vector<double> times_ms;
    std::string last_stats;

    // --- timed runs ---
    for (int r = 0; r < NUM_TIMED_RUNS; ++r) {
        HashTableDictionary table(M, probeType, doCompact);

        auto start = std::chrono::high_resolution_clock::now();
        replay_ops(table, ops);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> diff = end - start;
        times_ms.push_back(diff.count());

        // Grab CSV stats from the last run
        last_stats = table.csvStats();
    }

    double med_ms = median_of(times_ms);
    std::size_t ops_total = ops.size();

    // Seed is fixed to 23 for provided LRU traces
    int seed = 23;

    std::string impl_name =
        (probeType == HashTableDictionary::SINGLE)
        ? "hash_map_single"
        : "hash_map_double";

    std::cout << impl_name << ","
              << "lru_profile" << ","
              << tracePath << ","
              << N << ","
              << seed << ","
              << med_ms << ","
              << ops_total << ","
              << last_stats
              << "\n";
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr
            << "Usage: " << argv[0]
            << " <trace_directory>\n"
            << "Example: " << argv[0]
            << " lru_profile_trace_files\n";
        return 1;
    }

    std::string traceDir = argv[1];

    // Ns we care about. You already have 1024 and 2048;
    // you can add more when you get more trace files.
    std::vector<std::size_t> Ns = {
        1024,
        2048,
        4096,
        8192,
        16384,
        32768,
        65536,
        131072,
        262144,
        524288,
        1048576
    };

    // CSV header
    std::cout << "impl,profile,trace_path,N,seed,elapsed_ms,ops_total,"
              << HashTableDictionary::csvStatsHeader()
              << "\n";

    for (std::size_t N : Ns) {
        // Build the expected trace file name, e.g.:
        //   <traceDir>/lru_profile_N_1024_S_23.trace
        std::string tracePath = traceDir +
            "/lru_profile_N_" + std::to_string(N) + "_S_23.trace";

        // Try SINGLE probing with compaction on
        run_condition(tracePath, HashTableDictionary::SINGLE, true);

        // Try DOUBLE probing with compaction on
        run_condition(tracePath, HashTableDictionary::DOUBLE, true);
    }

    return 0;
}
