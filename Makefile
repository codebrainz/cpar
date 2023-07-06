cxxflags := $(CPPFLAGS) -Isrc $(CXXFLAGS) -g -O0 -std=c++17 -Wall -Wextra
ldflags := $(LDFLAGS)

sources = tests/catch_amalgamated.cpp tests/test.cpp
objects = $(sources:.cpp=.o)
depends = $(sources:.cpp=.d)

test: $(objects)
	$(CXX) $(strip $(cxxflags) -g -O0 -o $@ $(objects) $(ldflags))
	./test

.cpp.o:
	$(CXX) $(strip $(cxxflags) -c -MMD -o $@ $<)

clean:
	$(RM) src/*.[do] test

-include $(depends)

.PHONY: clean test
