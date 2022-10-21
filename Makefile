NAME := xtrlock-pam

CC ?= gcc

CFLAGS ?= -DNDEBUG -O2
LDFLAGS := -lX11 -lpam

SRC := $(wildcard *.c)
OBJ := $(patsubst %.c,%.c.o,$(SRC))

.PHONY: all clean install

all: $(NAME)

-include *.d

$(NAME): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

%.c.o: %.c %.h Makefile
	$(CC) $(CFLAGS) -MMD -o $@ -c $<

%.c.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -o $@ -c $<

clean:
	-@rm -f *.o *.d xtrlock-pam

install: all
	@mkdir -p $(PREFIX)/usr/bin/
	@cp xtrlock-pam $(PREFIX)/usr/bin/xtrlock-pam
	@mkdir -p $(PREFIX)/usr/local/share/
	@cp -r bitmaps/ $(PREFIX)/usr/local/share/xtrlock-pam-bitmaps/
