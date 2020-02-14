CXX = g++
FLAGS = -std=c++17
INCLUDE = -I .
OPTIM = -O3 -march=native
WARN = -Wall -Wextra -pedantic
ALL = $(FLAGS) $(INCLUDE) $(OPTIM) $(WARN)

default:
	$(CXX) $(ALL) -c TwinPeaks/Atoms.hpp
	$(CXX) $(ALL) -c TwinPeaks/Constraints.hpp
	$(CXX) $(ALL) -c TwinPeaks/Example.hpp
	$(CXX) $(ALL) -c TwinPeaks/Misc.hpp
	$(CXX) $(ALL) -c TwinPeaks/RNG.hpp
	$(CXX) $(ALL) -c TwinPeaks/Ordering.hpp
	$(CXX) $(ALL) -c TwinPeaks/Potts.hpp
	$(CXX) $(ALL) -c TwinPeaks/RunOptions.hpp
	$(CXX) $(ALL) -c TwinPeaks/Sampler.hpp
	$(CXX) $(ALL) -c main.cpp
	$(CXX) -o main main.o -lsqlite3
	rm -f main.o TwinPeaks/*.gch

tidy:
	clang-tidy main.cpp -- $(INCLUDE)
