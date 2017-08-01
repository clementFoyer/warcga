CC=gcc
CFLAGS=-Wall -Wextra --std=c99 -g -O0 -I.
SRC=$(wildcard ./*.c)
HEADERS=$(wildcard ./*.h)

all: warcga
warcga: $(HEADERS) $(SRC:.c=.o)
clean:
	rm -rf warcga ./*.o
