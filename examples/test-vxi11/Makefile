
PROG = test

SRCS = main.c vxi11_xdr.c ../common/scpi-def.c
CFLAGS += -Wextra -Wmissing-prototypes -Wimplicit -I ../../libscpi/inc/ -I /usr/include/tirpc
LDFLAGS += -lm -ltirpc ../../libscpi/dist/libscpi.a -Wl,--as-needed

.PHONY: clean all

all: $(PROG)

OBJS = $(SRCS:.c=.o)

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(PROG) $(OBJS)
