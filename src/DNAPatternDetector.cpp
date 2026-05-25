
#include "DNAPatternDetector.h"
#include <omp.h>
#include <algorithm>

DNAPatternDetector::DNAPatternDetector(const std::string& dna,
                                       const std::string& pat,
                                       int threads)
    : dnaSequence(dna), pattern(pat), threadCount(threads) {}

std::vector<int> DNAPatternDetector::searchPattern() {
    std::vector<int> results;
    int seqLen  = static_cast<int>(dnaSequence.size());
    int patLen  = static_cast<int>(pattern.size());

    if (patLen == 0 || seqLen < patLen) {
        return results;
    }

    int searchRange = seqLen - patLen + 1;

    omp_set_num_threads(threadCount);

    // Use per-thread local vectors to avoid mutex / critical-section contention.
    int maxThreads = threadCount;
    std::vector<std::vector<int>> threadResults(maxThreads);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();

        #pragma omp for schedule(dynamic, 1024)
        for (int i = 0; i < searchRange; i++) {
            if (dnaSequence.compare(i, patLen, pattern) == 0) {
                threadResults[tid].push_back(i);
            }
        }
    }

    // Merge all thread-local results
    for (auto& local : threadResults) {
        results.insert(results.end(), local.begin(), local.end());
    }

    // Sort for deterministic, ordered output
    std::sort(results.begin(), results.end());

    return results;
}
