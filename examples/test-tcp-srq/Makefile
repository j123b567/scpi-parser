
PROG = test

SRCS = main.c ../common/scpi-def.c
CFLAGS += -Wextra -I ../../libscpi/inc/
LDFLAGS += ../../libscpi/dist/libscpi.a


all: $(PROG)

OBJS = $(SRCS:.c=.o)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

clean:
	$(RM) $(PROG) $(OBJS)
