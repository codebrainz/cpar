cxxflags := $(CPPFLAGS) $(CXXFLAGS) -g -O0 -std=c++17 -Wall -Wextra
ldflags := $(LDFLAGS)

sources = catch_amalgamated.cpp test2.cpp
objects = $(sources:.cpp=.o)
depends = $(sources:.cpp=.d)

test: $(objects)
	$(CXX) $(strip $(cxxflags) -g -O0 -o $@ $(objects) $(ldflags))
	./test

.cpp.o:
	$(CXX) $(strip $(cxxflags) -c -MMD -o $@ $<)

clean:
	$(RM) *.[do] test

-include $(depends)

.PHONY: clean test
