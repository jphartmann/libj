#
.PHONY: clean install

CFLAGS:=-g -Wall -Werror -Wno-unknown-pragmas -Wredundant-decls -Wshadow \
	-Wcast-align -fPIC

PRF:=${HOME}
SRC:=jfile jdump
S:=${shell uname -s}
P:=${shell uname -p}
O:=obj/$S/$P

OBJS:=${addprefix $O/,${addsuffix .o,${SRC}}}

all: $O/libjx.a
clean:
	rm -f $O/*

install: | ${PRF}/include ${PRF}/lib
install: ${PRF}/include/libjx.h ${PRF}/lib/libjx.a

${PRF}/include/libjx.h: libjx.h
	cp -p $< $@

${PRF}/lib/libjx.a: $O/libjx.a
	cp -p $< $@

${OBJS}: | $O
$O:
	mkdir -p $O

$O/%.o: %.c
	${CC} -c -o $@ $< ${CFLAGS}

$O/libjx.a: ${OBJS}
	rm -f $@
	ar q $@ ${OBJS}
	ranlib $@
