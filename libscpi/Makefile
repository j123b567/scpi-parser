LIBNAME = scpi

CFLAGS += -Wextra -Wmissing-prototypes -Wimplicit -g -Iinc
LDFLAGS += -Wl,--as-needed
#TESTCFLAGS += $(CFLAGS) `pkg-config --cflags cunit`
#TESTLDFLAGS += $(LDFLAGS) `pkg-config --libs cunit`
TESTCFLAGS += $(CFLAGS)
TESTLDFLAGS += $(LDFLAGS) -lcunit

OBJDIR=obj
DISTDIR=dist
TESTDIR=test

STATICLIBFLAGS = rcs

STATICLIB = lib$(LIBNAME).a

SRCS = $(addprefix src/, \
	error.c fifo.c ieee488.c \
	minimal.c parser.c units.c utils.c \
	lexer.c \
	)

OBJS = $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.c=.o)))

HDRS = $(addprefix inc/scpi/, \
	scpi.h constants.h error.h \
	ieee488.h minimal.h parser.h types.h units.h \
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

.PHONY: all clean static test

all: static

static: $(DISTDIR)/$(STATICLIB)

clean:
	$(RM) -r $(OBJDIR) $(DISTDIR) $(TESTS_BINS) $(TESTS_OBJS)

test: $(TESTS_BINS)
	$(TESTS_BINS:.test=.test &&) true

$(OBJDIR):
	mkdir -p $@

$(DISTDIR):
	mkdir -p $@

$(OBJDIR)/%.o: src/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

$(DISTDIR)/$(STATICLIB): $(OBJS)
	$(AR) $(STATICLIBFLAGS) $(DISTDIR)/$(STATICLIB) $(OBJS)


$(OBJS): $(HDRS) $(DISTDIR) $(OBJDIR) $(SRCS)


$(TESTDIR)/%.o: $(TESTDIR)/%.c
	$(CC) -c $(TESTCFLAGS) $(CPPFLAGS) -o $@ $<

$(TESTDIR)/%.test: $(TESTDIR)/%.o $(DISTDIR)/$(STATICLIB)
	$(CC) $< -o $@ $(DISTDIR)/$(STATICLIB) $(TESTLDFLAGS)



