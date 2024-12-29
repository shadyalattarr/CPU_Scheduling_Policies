# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17

# Target executables
TARGETS = lab6 fcfs rr spn hrrn fb_2i fb_1 srt aging

# Source files
SRC_LAB6 = lab6.cpp
SRC_FCFS = FCFS.cpp
SRC_RR = RR.cpp
SRC_SPN = myspn.cpp
SRC_HRRN = myhrrn.cpp
SRC_FB_1 = myfb_1.cpp
SRC_FB_2I = myfb_2i.cpp
SRC_SRT = SRT.cpp
SRC_AGING = aging.cpp

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

fb_2i: $(SRC_FB_2I)
	$(CXX) $(CXXFLAGS) -o fb_2i $(SRC_FB_2I)

fb_1: $(SRC_FB_1)
	$(CXX) $(CXXFLAGS) -o fb_1 $(SRC_FB_1)

hrrn: $(SRC_HRRN)
	$(CXX) $(CXXFLAGS) -o hrrn $(SRC_HRRN)

srt: $(SRC_SRT)
	$(CXX) $(CXXFLAGS) -o srt $(SRC_SRT)

aging: $(SRC_AGING)
	$(CXX) $(CXXFLAGS) -o aging $(SRC_AGING)

# Clean rule to remove compiled files
clean:
	rm -f $(TARGETS)

# Run the lab6 program
run: all
	./lab6


#save for testing
# cat ./testcases/0-a-input.txt | ./lab6 | diff ./testcases/0-a-output.txt -