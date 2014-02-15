#CFLAGS=-Wall -g
CFLAGS=-Wall -O9

bf: main.o machine.o memory.o
	$(CC) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $^

clean: main.o machine.o memory.o
	rm -f $^
