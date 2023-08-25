CC = gcc
CCOPTS=--std=gnu99 -Wall -D_LIST_DEBUG_ 
AR=ar
RM=rm

OBJS= bit_map.o\
    buddy_allocator.o\
	pseudo_malloc.o\
	test.o\

HEADERS= bit_map.h\
	buddy_allocator.h\
	pseudo_malloc.h\
	allocator_common.h\

LIBS=libbuddy.a

BINS=pseudo_malloc_test

.phony: clean all


all:	$(LIBS) $(BINS)

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@  $<

libbuddy.a: $(OBJS) 
	$(AR) -rcs $@ $^

pseudo_malloc_test: test.o $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^ -lm
	$(RM) $(OBJS)

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)
