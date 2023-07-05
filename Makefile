test: cpar-test
	./cpar-test

cpar-test: src/test.c src/cpar.h
	$(CC) $(strip $(CPPFLAGS) $(CFLAGS) -g -O0 -o $@ $^ $(LDFLAGS))

clean:
	$(RM) cpar-test

.PHONY: clean test
