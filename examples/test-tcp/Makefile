
PROG = test

SRCS = main.c ../common/scpi-def.c
CFLAGS += -Wextra -Wmissing-prototypes -Wimplicit -I ../../libscpi/inc/
LDFLAGS += -lm ../../libscpi/dist/libscpi.a -Wl,--as-needed

.PHONY: clean all

all: $(PROG)

OBJS = $(SRCS:.c=.o)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(PROG) $(OBJS)
