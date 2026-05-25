
#ifndef DNA_PATTERN_DETECTOR_H
#define DNA_PATTERN_DETECTOR_H

#include <string>
#include <vector>

class DNAPatternDetector {
private:
    std::string dnaSequence;
    std::string pattern;
    int threadCount;

public:
    DNAPatternDetector(const std::string& dna,
                       const std::string& pat,
                       int threads);

    std::vector<int> searchPattern();
};

#endif
