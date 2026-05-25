
#include "FastaParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cctype>

std::string FastaParser::cleanSequence(const std::string& raw) {
    std::string cleaned;
    cleaned.reserve(raw.size());
    for (char c : raw) {
        if (std::isalpha(static_cast<unsigned char>(c))) {
            cleaned += static_cast<char>(
                std::toupper(static_cast<unsigned char>(c)));
        }
    }
    return cleaned;
}

std::vector<FastaRecord> FastaParser::parse(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    std::string content(
        (std::istreambuf_iterator<char>(file)),
         std::istreambuf_iterator<char>());
    return parseString(content);
}

std::vector<FastaRecord> FastaParser::parseString(const std::string& content) {
    std::vector<FastaRecord> records;
    std::istringstream stream(content);
    std::string line;

    // --- Check if this is FASTA format (any line starts with '>') ---
    bool hasFastaHeaders = false;
    {
        std::istringstream check(content);
        std::string tmp;
        while (std::getline(check, tmp)) {
            // Strip trailing \r
            if (!tmp.empty() && tmp.back() == '\r') tmp.pop_back();
            if (!tmp.empty() && tmp[0] == '>') {
                hasFastaHeaders = true;
                break;
            }
        }
    }

    // --- Plain-text fallback: treat entire file as one sequence ---
    if (!hasFastaHeaders) {
        FastaRecord record;
        record.header = "Unnamed_Sequence";
        std::string seqAccum;
        std::istringstream plain(content);
        while (std::getline(plain, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (line.empty() || line[0] == ';') continue;
            seqAccum += line;
        }
        record.sequence = cleanSequence(seqAccum);
        if (!record.sequence.empty()) {
            records.push_back(record);
        }
        return records;
    }

    // --- FASTA format parsing ---
    FastaRecord current;
    bool inRecord = false;

    while (std::getline(stream, line)) {
        // Handle Windows line endings
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Skip empty lines and comment lines
        if (line.empty() || line[0] == ';') continue;

        if (line[0] == '>') {
            // Save the previous record
            if (inRecord) {
                current.sequence = cleanSequence(current.sequence);
                if (!current.sequence.empty()) {
                    records.push_back(current);
                }
            }
            // Start a new record
            current = FastaRecord();
            current.header = line.substr(1);
            // Trim leading whitespace from header
            size_t start = current.header.find_first_not_of(" \t");
            if (start != std::string::npos) {
                current.header = current.header.substr(start);
            }
            inRecord = true;
        } else if (inRecord) {
            current.sequence += line;
        }
    }

    // Don't forget the last record
    if (inRecord) {
        current.sequence = cleanSequence(current.sequence);
        if (!current.sequence.empty()) {
            records.push_back(current);
        }
    }

    return records;
}
