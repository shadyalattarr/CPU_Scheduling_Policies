# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17

# Target executables
TARGETS = lab6 fcfs spn rr

# Source files
SRC_LAB6 = lab6.cpp
SRC_FCFS = FCFS.cpp
SRC_RR = RR.cpp
SRC_SPN = spn.cpp

# Default rule to build all executables
all: $(TARGETS)

# Rule to build lab6 executable
lab6: $(SRC_LAB6)
	$(CXX) $(CXXFLAGS) -o lab6 $(SRC_LAB6)

# Rule to build fcfs executable
fcfs: $(SRC_FCFS)
	$(CXX) $(CXXFLAGS) -o fcfs $(SRC_FCFS)

rr: $(SRC_RR)
	$(CXX) $(CXXFLAGS) -o rr $(SRC_RR)

spn: $(SRC_SPN)
	$(CXX) $(CXXFLAGS) -o spn $(SRC_SPN)

# Clean rule to remove compiled files
clean:
	rm -f $(TARGETS)

# Run the lab6 program
run: all
	./lab6
