CC := gcc
CFLAGS := -O0 -g -Wall
LDLIBS := -lpthread

all: microbench1 microbench2 microbench3

microbench1: microbench1.o
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

microbench2: microbench2.o
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

microbench3: microbench3.o
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

clean:
	rm -f microbench1 microbench2 microbench3 *.o

.PHONY: all clean
