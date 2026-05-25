# ðŸ§¬ DNA Pattern Detector

![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=cplusplus&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![Multithreading](https://img.shields.io/badge/Multithreading-FF6F00?style=for-the-badge&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)

> **A high-performance multithreaded DNA pattern matching system built with C++ on Linux â€” leveraging parallel processing for fast pattern detection in genetic sequences.**

---

## ðŸŽ¯ Overview

DNA Pattern Detector is a command-line tool that searches for specific nucleotide patterns within DNA sequences. It uses **multithreading** to parallelize the search across large datasets, achieving significant speedup on multi-core systems. Built on Ubuntu Linux using POSIX threads.

## âœ¨ Features

- ðŸ”¬ **Pattern Matching** â€” Search for specific DNA patterns (A, T, G, C sequences)
- âš¡ **Multithreaded** â€” Parallel processing using POSIX threads for high performance
- ðŸ“Š **Performance Metrics** â€” Reports execution time and speedup vs single-threaded
- ðŸ“ **File I/O** â€” Read DNA sequences from files and output results
- ðŸ”§ **Configurable** â€” Adjustable thread count and search parameters
- ðŸ§ **Linux Native** â€” Built for Ubuntu/Linux environments

## ðŸ› ï¸ Tech Stack

| Technology | Purpose |
|-----------|---------|
| C++ | Core language |
| POSIX Threads (pthreads) | Multithreading & parallelism |
| Linux/Ubuntu | Target platform |
| Makefile | Build system |

## ðŸš€ Getting Started

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install g++ make

# Or any Linux with GCC
g++ --version  # Verify GCC is installed
```

### Build & Run
```bash
# Clone the repository
git clone https://github.com/KHALEDNOAMAN/DNA_Pattern_Detector.git
cd DNA_Pattern_Detector

# Compile
g++ -o dna_detector main.cpp -lpthread -O2

# Run
./dna_detector <sequence_file> <pattern> [num_threads]
```

### Example
```bash
# Search for pattern "ATCGATCG" using 4 threads
./dna_detector sample_dna.txt ATCGATCG 4
```

## ðŸ—ï¸ Architecture

```
DNA_Pattern_Detector/
â”œâ”€â”€ main.cpp           # Entry point & thread management
â”œâ”€â”€ pattern_search.h   # Pattern matching algorithms
â”œâ”€â”€ file_handler.h     # DNA file I/O operations
â”œâ”€â”€ thread_pool.h      # Thread pool implementation
â”œâ”€â”€ sample_dna.txt     # Sample DNA sequence data
â”œâ”€â”€ Makefile           # Build configuration
â””â”€â”€ README.md
```

### How It Works

1. **Read** â€” Load DNA sequence from input file
2. **Split** â€” Divide sequence into chunks for parallel processing
3. **Search** â€” Each thread searches its chunk for the target pattern
4. **Merge** â€” Combine results from all threads
5. **Report** â€” Output pattern locations and performance metrics

```
â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
â”‚              DNA Sequence Input              â”‚
â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”¬â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
                   â”‚
          â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”¼â”€â”€â”€â”€â”€â”€â”€â”€â”
          â–¼        â–¼        â–¼
     â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”
     â”‚Thread 1â”‚â”‚Thread 2â”‚â”‚Thread Nâ”‚
     â”‚Chunk 1 â”‚â”‚Chunk 2 â”‚â”‚Chunk N â”‚
     â””â”€â”€â”€â”¬â”€â”€â”€â”€â”˜â””â”€â”€â”€â”¬â”€â”€â”€â”€â”˜â””â”€â”€â”€â”¬â”€â”€â”€â”€â”˜
         â”‚         â”‚         â”‚
         â””â”€â”€â”€â”€â”¬â”€â”€â”€â”€â”˜         â”‚
              â””â”€â”€â”€â”€â”€â”€â”¬â”€â”€â”€â”€â”€â”€â”€â”˜
                     â–¼
          â”Œâ”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”
          â”‚  Merged Results   â”‚
          â”‚  + Performance    â”‚
          â””â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”˜
```

## ðŸ“ˆ Performance

| Threads | Speedup (vs 1 thread) |
|---------|----------------------|
| 1 | 1.0x (baseline) |
| 2 | ~1.8x |
| 4 | ~3.5x |
| 8 | ~6.2x |

*Results may vary based on hardware and sequence length.*

## ðŸ“„ License

This project is licensed under the MIT License.

## ðŸ‘¤ Author

**Khaled Noaman** â€” Computer Engineering Student at Istanbul Arel University

- [GitHub](https://github.com/KhaledNoaman)
- [LinkedIn](https://www.linkedin.com/in/khalednoaman1/)
