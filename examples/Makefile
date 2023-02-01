.PHONY: clean all cli tcp

all: cli tcp

cli:
	$(MAKE) -C test-interactive
	$(MAKE) -C test-interactive-cxx
	$(MAKE) -C test-parser

tcp:
	$(MAKE) -C test-tcp
	$(MAKE) -C test-tcp-srq

clean:
	$(MAKE) clean -C test-interactive
	$(MAKE) clean -C test-interactive-cxx
	$(MAKE) clean -C test-parser
	$(MAKE) clean -C test-tcp
	$(MAKE) clean -C test-tcp-srq


