
CC := cc
LD :=cc

OBJFILES := $(patsubst %.c, %.o, $(wildcard *.c))

all: rbtree

%.o: %.c
	${CC} -g -c -O0 $^ -o $@

rbtree: $(OBJFILES)
	${LD} $^ -o $@

clean: rbtree $(OBJFILES)
	@rm -rf $^
