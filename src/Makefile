CC=clang
CFLAGS=-Wall -Wextra -std=c11
SRCS:=$(wildcard *.c)
OBJECTS:=$(SRCS:.c=.o)

zcc: ${OBJECTS}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJECTS} -o $@

${OBJECTS}: zcc.h

.PHONY: test clean
test: zcc
	./test.sh

clean:
	rm -f ./zcc ./*.o temp*
