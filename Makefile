CC = gcc
CFLAGS = -Wall -Wextra -std=c17

all: stat myls mytail myfind

stat: stat.c
	$(CC) $(CFLAGS) -o stat stat.c

myls: myls.c
	$(CC) $(CFLAGS) -o myls myls.c

mytail: mytail.c
	$(CC) $(CFLAGS) -o mytail mytail.c

myfind: myfind.c
	$(CC) $(CFLAGS) -o myfind myfind.c

clean:
	rm -f stat myls mytail myfind
