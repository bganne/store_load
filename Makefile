CFLAGS:= -O2 -g -Wextra -march=core2 -fopenmp -DNDEBUG
LDFLAGS:= -fopenmp

all: store_load

store_load: store_load.c

clean:
	$(RM) store_load

.PHONY: all clean
