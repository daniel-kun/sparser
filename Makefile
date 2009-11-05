CC=gcc
CFLAGS=-W -Wall -pedantic -ansi
OBJ=main.o sparser.o
TARGET=sparser
LIBS=

all: ${OBJ}
	${CC} -o ${TARGET} ${LIBS} ${OBJ}

clean:
	@rm -vf ${OBJ}
	@rm -vf ${TARGET}
