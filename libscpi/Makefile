VERSION = 2.1.0
LIBNAME = scpi

CFLAGS += -Wextra -Wmissing-prototypes -Wimplicit -Iinc
CFLAGS_SHARED += $(CFLAGS) -fPIC
LDFLAGS += -lm -Wl,--as-needed
#TESTCFLAGS += $(CFLAGS) `pkg-config --cflags cunit`
#TESTLDFLAGS += $(LDFLAGS) `pkg-config --libs cunit`
TESTCFLAGS += $(CFLAGS)
TESTLDFLAGS += $(LDFLAGS) -lcunit

OBJDIR=obj
OBJDIR_STATIC=$(OBJDIR)/static
OBJDIR_SHARED=$(OBJDIR)/shared
DISTDIR=dist
TESTDIR=test

PREFIX := $(DESTDIR)/usr/local
LIBDIR := $(PREFIX)/lib
INCDIR := $(PREFIX)/include

STATICLIBFLAGS = rcs
SHAREDLIBFLAGS = $(LDFLAGS) -shared -Wl,-soname,$(SHAREDLIB)

STATICLIB = lib$(LIBNAME).a
SHAREDLIB = lib$(LIBNAME).so
SHAREDLIBVER = $(SHAREDLIB).$(VERSION)

SRCS = $(addprefix src/, \
	error.c fifo.c ieee488.c \
	minimal.c parser.c units.c utils.c \
	lexer.c expression.c \
	)

OBJS_STATIC = $(addprefix $(OBJDIR_STATIC)/, $(notdir $(SRCS:.c=.o)))
OBJS_SHARED = $(addprefix $(OBJDIR_SHARED)/, $(notdir $(SRCS:.c=.o)))

HDRS = $(addprefix inc/scpi/, \
	scpi.h constants.h error.h \
	ieee488.h minimal.h parser.h types.h units.h \
	expression.h \
	) \
	$(addprefix src/, \
	lexer_private.h utils_private.h fifo_private.h \
	parser_private.h \
	) \


TESTS = $(addprefix $(TESTDIR)/, \
	test_fifo.c test_scpi_utils.c test_lexer_parser.c test_parser.c\
	)

TESTS_OBJS = $(TESTS:.c=.o)
TESTS_BINS = $(TESTS_OBJS:.o=.test)

.PHONY: all clean static shared test install

all: static shared

static: $(DISTDIR)/$(STATICLIB)

shared: $(DISTDIR)/$(SHAREDLIBVER)

clean:
	$(RM) -r $(OBJDIR) $(DISTDIR) $(TESTS_BINS) $(TESTS_OBJS)

test: $(TESTS_BINS)
	$(TESTS_BINS:.test=.test &&) true

install: $(DISTDIR)/$(STATICLIB) $(DISTDIR)/$(SHAREDLIBVER)
	test -d $(PREFIX) || mkdir $(PREFIX)
	test -d $(LIBDIR) || mkdir $(LIBDIR)
	test -d $(INCDIR) || mkdir $(INCDIR)
	test -d $(INCDIR)/scpi || mkdir $(INCDIR)/scpi
	install -m 0644 $(DISTDIR)/$(STATICLIB) $(LIBDIR)
	install -m 0644 $(DISTDIR)/$(SHAREDLIBVER) $(LIBDIR)
	install -m 0644 inc/scpi/*.h $(INCDIR)/scpi

$(OBJDIR_STATIC):
	mkdir -p $@

$(OBJDIR_SHARED):
	mkdir -p $@

$(DISTDIR):
	mkdir -p $@

$(OBJDIR_STATIC)/%.o: src/%.c $(HDRS) | $(OBJDIR_STATIC)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(OBJDIR_SHARED)/%.o: src/%.c $(HDRS) | $(OBJDIR_SHARED)
	$(CC) -c $(CFLAGS_SHARED) $(CPPFLAGS) -o $@ $<

$(DISTDIR)/$(STATICLIB): $(OBJS_STATIC) | $(DISTDIR)
	$(AR) $(STATICLIBFLAGS) $(DISTDIR)/$(STATICLIB) $(OBJS_STATIC)

$(DISTDIR)/$(SHAREDLIBVER): $(OBJS_SHARED) | $(DISTDIR)
	$(CC) $(SHAREDLIBFLAGS) -o $(DISTDIR)/$(SHAREDLIBVER) $(OBJS_SHARED)

$(TESTDIR)/%.o: $(TESTDIR)/%.c
	$(CC) -c $(TESTCFLAGS) $(CPPFLAGS) -o $@ $<

$(TESTDIR)/%.test: $(TESTDIR)/%.o $(DISTDIR)/$(STATICLIB)
	$(CC) $< -o $@ $(DISTDIR)/$(STATICLIB) $(TESTLDFLAGS)



