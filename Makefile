test: cpar-test
	./cpar-test

cpar-test: test.cpp cpar.h
	$(CXX) $(strip $(CPPFLAGS) $(CXXFLAGS) -g -O0 -o $@ $^ $(LDFLAGS))

clean:
	$(RM) cpar-test

.PHONY: clean test
