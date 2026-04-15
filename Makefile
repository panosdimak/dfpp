CXX = g++
CXXFLAGS = -std=c++23 -Wall -Wextra -pedantic

dfpp: main.o fs_info.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f dfpp *.o
