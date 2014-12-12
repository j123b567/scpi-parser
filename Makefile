all:
	$(MAKE) -C libscpi
	$(MAKE) -C examples

clean:
	$(MAKE) clean -C libscpi
	$(MAKE) clean -C examples

build-tests:
	$(MAKE) test -C libscpi