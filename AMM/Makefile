CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
SRC = $(wildcard src/*.cpp)
HDR = $(wildcard src/*.h)

amm: $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(SRC) -o $@

debug: $(SRC) $(HDR)
	$(CXX) -std=c++17 -O0 -g -fsanitize=address,undefined -Wall -Wextra $(SRC) -o amm_debug

clean:
	rm -f amm amm_debug

.PHONY: clean debug
