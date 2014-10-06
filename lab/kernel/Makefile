CC:=gcc
CFLAGS:= -O0 -Wall -fopenmp
LDLIBS:= -lgomp
all:serial parallel

serial:serial.o
	$(CC) $^ -o $@ $(LDLIBS)

parallel:parallel.o
	$(CC) $^ -o $@ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f serial *.o

.PHONY:all clean
