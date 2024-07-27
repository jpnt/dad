VERSION = 1.0
NAME = dad

PREFIX ?= /usr/local
BINDIR ?= ${PREFIX}/bin

INCS = -I. -I${PREFIX}/include
LIBS =

CC = gcc
CFLAGS += -std=c99 -pedantic -Wall -Wextra -Os ${INCS} -DVERSION=\"${VERSION}\"
LDFLAGS += ${LIBS}

EXEC = ${NAME}

SRC = ${NAME}.c
OBJ = ${SRC:.c=.o}

all: ${NAME}

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h

config.h:
	cp config.def.h $@

${NAME}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -fv ${NAME} ${OBJ}

install: all
	install -Dm755 ${NAME} ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall:
	rm -fv ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONE: all clean install uninstall
