
#include <iostream>
#include <chrono>
#include "DNAPatternDetector.h"
#include "FastaParser.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: ./dna_detector <file> <pattern> <threads>\n";
        std::cout << "  Supports both FASTA (.fasta, .fa) and plain text files.\n";
        return 1;
    }

    std::string filepath = argv[1];
    std::string pattern  = argv[2];
    int threads          = std::stoi(argv[3]);

    // --- Parse input file (auto-detects FASTA vs plain text) ---
    std::vector<FastaRecord> records;
    try {
        records = FastaParser::parse(filepath);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    if (records.empty()) {
        std::cerr << "No sequences found in file.\n";
        return 1;
    }

    std::cout << "\n=== DNA Pattern Detector (OpenMP) ===\n";
    std::cout << "File:      " << filepath << "\n";
    std::cout << "Pattern:   " << pattern << "\n";
    std::cout << "Threads:   " << threads << "\n";
    std::cout << "Sequences: " << records.size() << "\n";

    auto totalStart = std::chrono::high_resolution_clock::now();
    int totalMatches = 0;

    for (size_t s = 0; s < records.size(); s++) {
        const auto& rec = records[s];

        std::cout << "\n--- Sequence " << (s + 1)
                  << ": " << rec.header
                  << " (" << rec.sequence.size() << " bp) ---\n";

        DNAPatternDetector detector(rec.sequence, pattern, threads);

        auto start   = std::chrono::high_resolution_clock::now();
        std::vector<int> matches = detector.searchPattern();
        auto end     = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Matches Found: " << matches.size() << "\n";
        std::cout << "Positions: ";
        for (size_t i = 0; i < matches.size(); i++) {
            std::cout << matches[i];
            if (i < matches.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
        std::cout << "Time: " << elapsed.count() << " ms\n";

        totalMatches += static_cast<int>(matches.size());
    }

    auto totalEnd = std::chrono::high_resolution_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        totalEnd - totalStart);

    std::cout << "\n=== Summary ===\n";
    std::cout << "Total Matches: " << totalMatches << "\n";
    std::cout << "Total Time:    " << totalDuration.count() << " ms\n";

    return 0;
}
