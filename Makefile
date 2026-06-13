CXX = g++

WARNINGS = -Wall -Wextra -pedantic \
           -Wshadow -Wconversion -Wsign-conversion \
           -Wold-style-cast -Wcast-qual -Wdouble-promotion

CXXFLAGS = -std=c++23 $(WARNINGS) -MMD -MP

SRCS = main.cpp fs_info.cpp format.cpp table.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

release: CXXFLAGS += -O2
release: LDFLAGS += -s
release: dfpp

debug: CXXFLAGS += -O0 -g -fsanitize=address,undefined -fno-omit-frame-pointer
debug: LDFLAGS += -fsanitize=address,undefined
debug: dfpp

dfpp: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f dfpp $(OBJS) $(DEPS)

-include $(DEPS)

.PHONY: release debug clean
