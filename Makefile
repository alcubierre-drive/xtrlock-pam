NAME := xtrlock-pam
CC := gcc
LD := gcc

CFLAGS := -DNDEBUG -O2
LDFLAGS := -lX11 -lpam

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.c.o,$(SRC))

.PHONY: all clean

all: $(NAME)

-include *.d

$(NAME): $(OBJ)
	$(LD) $^ $(LDFLAGS) -o $@

%.c.o: %.c %.h Makefile
	$(CC) $(CFLAGS) -MMD -o $@ -c $<

%.c.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -o $@ -c $<

clean:
	-@rm -f *.o *.d xtrlock-pam
