test: cpar-test
	./cpar-test

cpar-test: src/test.cpp src/cpar.h
	$(CXX) $(strip $(CPPFLAGS) $(CXXFLAGS) -g -O0 -o $@ $^ $(LDFLAGS))

clean:
	$(RM) cpar-test

.PHONY: clean test
