
#ifndef FASTA_PARSER_H
#define FASTA_PARSER_H

#include <string>
#include <vector>

/**
 * Represents a single record from a FASTA file.
 * Contains the header line (without '>') and the concatenated sequence.
 */
struct FastaRecord {
    std::string header;     // e.g. "chr1 Human chromosome 1"
    std::string sequence;   // concatenated, uppercase, cleaned sequence
};

/**
 * Parses FASTA-formatted files (.fasta, .fa) and plain-text DNA files.
 *
 * FASTA format:
 *   >header_line
 *   SEQUENCE_DATA
 *   SEQUENCE_DATA_CONTINUED
 *   >next_header
 *   ...
 *
 * If no '>' headers are found, the entire file is treated as a single
 * unnamed sequence (backward compatible with plain text DNA files).
 */
class FastaParser {
public:
    /**
     * Parse a FASTA or plain-text DNA file from disk.
     * @param filepath Path to the input file.
     * @return Vector of FastaRecord entries.
     * @throws std::runtime_error if the file cannot be opened.
     */
    static std::vector<FastaRecord> parse(const std::string& filepath);

    /**
     * Parse FASTA-formatted content from an in-memory string.
     * Useful for GUI text-area input.
     * @param content The raw file content as a string.
     * @return Vector of FastaRecord entries.
     */
    static std::vector<FastaRecord> parseString(const std::string& content);

private:
    /**
     * Clean a raw sequence string: remove non-alpha characters,
     * convert to uppercase.
     */
    static std::string cleanSequence(const std::string& raw);
};

#endif
