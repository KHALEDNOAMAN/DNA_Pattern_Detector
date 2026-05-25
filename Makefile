
CXX      = g++
CXXFLAGS = -std=c++17 -O3 -fopenmp -Wall

# Source files
CORE_SRC = src/DNAPatternDetector.cpp src/FastaParser.cpp
CLI_SRC  = src/main.cpp $(CORE_SRC)
GUI_SRC  = src/gui/gui_main.cpp src/gui/MainWindow.cpp $(CORE_SRC)

# GTK3 flags (only needed for gui target)
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0 2>/dev/null)
GTK_LIBS   = $(shell pkg-config --libs   gtk+-3.0 2>/dev/null)

# Targets
CLI_TARGET = dna_detector
GUI_TARGET = dna_detector_gui

# ---- Default: build CLI only ----
all: $(CLI_TARGET)

$(CLI_TARGET): $(CLI_SRC)
	$(CXX) $(CXXFLAGS) $(CLI_SRC) -o $(CLI_TARGET)

# ---- GUI: requires libgtk-3-dev ----
gui: $(GUI_TARGET)

$(GUI_TARGET): $(GUI_SRC)
	$(CXX) $(CXXFLAGS) $(GTK_CFLAGS) $(GUI_SRC) -o $(GUI_TARGET) $(GTK_LIBS)

# ---- Build everything ----
full: $(CLI_TARGET) $(GUI_TARGET)

# ---- Clean ----
clean:
	rm -f $(CLI_TARGET) $(GUI_TARGET)

.PHONY: all gui full clean
