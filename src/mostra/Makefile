CC=gcc
CFLAGS=-O3 -g3  -W -Wall -Wextra -Wuninitialized -Wstrict-aliasing -Wno-format-security -Wno-unused-but-set-variable -Wno-unused-result
# Define as dependencias
DEPS = $(wildcard *.h)
# Define os ficheiros c a incluir
OBJ = $(wildcard *.c)


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all:
	$(CC) -o ../../bin/$(shell basename $(CURDIR)) *.c $(CFLAGS)
	
clean:
	rm bin/build
