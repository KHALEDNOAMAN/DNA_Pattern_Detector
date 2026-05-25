
# DNA Pattern Detector

A multithreaded C++ System Programming project for detecting DNA patterns in large genome sequences.

## Features
- **FASTA file parser** — reads standard `.fasta` / `.fa` files with multi-sequence support; auto-detects plain text files for backward compatibility
- **OpenMP parallel search** — pattern matching parallelized with OpenMP (`#pragma omp parallel for`) using thread-local result vectors and sorted output
- **GTK3 GUI** — graphical interface with dark theme, file browser, sequence selector, and split-pane results view
- **CLI mode** — command-line interface for scripting and batch usage
- Performance timing (per-sequence and total)
- Linux compatible (Ubuntu)
- Optimized for `-O3`

## Technologies
- C++17
- OpenMP
- GTK3 (GUI)
- STL

## Prerequisites

```bash
# Required: compiler with OpenMP support
sudo apt install g++ make

# Optional: for GUI
sudo apt install libgtk-3-dev
```

## Build

### CLI only (default)

```bash
make
```

### GUI only

```bash
make gui
```

### Build everything

```bash
make full
```

## Run

### CLI

```bash
./dna_detector <file> <pattern> <threads>
```

Supports both FASTA (`.fasta`, `.fa`) and plain text files.

#### Examples

```bash
# Plain text file
./dna_detector sample_dna.txt ACTG 4

# FASTA file with multiple sequences
./dna_detector sample.fasta GATTACA 8
```

### GUI

```bash
./dna_detector_gui
```

1. Click **Browse** to load a `.fasta` or `.txt` file
2. Enter a DNA pattern (e.g. `GATTACA`)
3. Adjust thread count
4. Click **Search Pattern**

## Output
- Match positions (per sequence)
- Total matches
- Execution time (per sequence and total)

## Project Structure

```
DNA_Pattern_Detector/
├── Makefile
├── README.md
├── sample_dna.txt           # plain text sample
├── sample.fasta             # multi-sequence FASTA sample
└── src/
    ├── DNAPatternDetector.h
    ├── DNAPatternDetector.cpp  # OpenMP pattern search
    ├── FastaParser.h
    ├── FastaParser.cpp         # FASTA / plain text parser
    ├── main.cpp                # CLI entry point
    └── gui/
        ├── MainWindow.h
        ├── MainWindow.cpp      # GTK3 GUI implementation
        └── gui_main.cpp        # GUI entry point
```

## Author
Khaled Noaman
