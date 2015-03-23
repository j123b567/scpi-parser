
VERSION = 0.3.0
LIBNAME = scpi


CFLAGS += -Wextra -O1 -g -fPIC -Iinc
LDFLAGS += -Wl,--as-needed
TESTFLAGS += -lcunit

OBJDIR=obj
DISTDIR=dist
TESTDIR=test

STATICLIBFLAGS = rcs
SHAREDLIBFLAGS = $(LDFLAGS) -shared -Wl,-soname


STATICLIB = lib$(LIBNAME).a
SHAREDLIB = lib$(LIBNAME).so

SHAREDLIBVER = $(SHAREDLIB).$(VERSION)

SRCS = $(addprefix src/, \
	debug.c error.c fifo.c ieee488.c \
	minimal.c parser.c units.c utils.c \
	)

OBJS = $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))

HDRS = $(addprefix inc/scpi/, \
	scpi.h constants.h debug.h error.h \
	fifo.h ieee488.h minimal.h parser.h \
	types.h units.h utils_private.h \
	)


TESTS = $(addprefix test/, \
	test_fifo.c test_scpi_utils.c test_lib.c \
	)

TESTS_OBJS = $(TESTS:.c=.o)
TESTS_BINS = $(TESTS_OBJS:.o=.test)

.PHONY: all clean static shared test

all: static shared


$(OBJDIR):
	mkdir -p $@

$(DISTDIR):
	mkdir -p $@

$(OBJDIR)/%.o: src/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(DISTDIR)/$(STATICLIB): $(OBJS)
	$(AR) $(STATICLIBFLAGS) $(DISTDIR)/$(STATICLIB) $(OBJS)

$(DISTDIR)/$(SHAREDLIBVER): $(OBJS)
	$(CC) $(SHAREDLIBFLAGS),$(SHAREDLIB) -o $(DISTDIR)/$(SHAREDLIBVER) $(OBJS)

$(DISTDIR)/$(SHAREDLIB): $(DISTDIR)/$(SHAREDLIBVER)
	$(RM) $(DISTDIR)/$(SHAREDLIB)
	ln -s $(SHAREDLIBVER) $(DISTDIR)/$(SHAREDLIB)

static: $(DISTDIR)/$(STATICLIB)

shared: $(DISTDIR)/$(SHAREDLIB)


$(OBJS): $(HDRS) $(DISTDIR) $(OBJDIR)

clean:
	$(RM) -r $(OBJDIR) $(DISTDIR) $(TESTS_BINS) $(TESTS_OBJS)

test: static $(TESTS_BINS)
	for t in $(TESTS_BINS); do ./$$t; done


$(TESTDIR)/%.o: $(TESTDIR)/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(TESTDIR)/%.test: $(TESTDIR)/%.o
	$(CC) $(TESTFLAGS) $< $(DISTDIR)/$(STATICLIB) -o $@ $(LDFLAGS)



