.PHONY: clean all test

all:
	$(MAKE) -C libscpi
	$(MAKE) -C examples

clean:
	$(MAKE) clean -C libscpi
	$(MAKE) clean -C examples

test:
	$(MAKE) test -C libscpi