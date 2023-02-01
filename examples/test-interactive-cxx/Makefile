
PROG = test

SRCS = main.cpp ../common-cxx/scpi-def.cpp
CPPFLAGS += -I ../../libscpi/inc/
CXXFLAGS += -Wextra
LDFLAGS += -lm ../../libscpi/dist/libscpi.a -Wl,--as-needed

.PHONY: clean all

all: $(PROG)

OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LDFLAGS)

clean:
	$(RM) $(PROG) $(OBJS)
