.PHONY: main lib test-all

all: build test

build: main

lib:
	$(MAKE) -C libsrc

main: lib
	$(MAKE) -C main

test: main
	$(MAKE) -C run

clean:
	$(MAKE) -C libsrc clean
	$(MAKE) -C main clean
	$(MAKE) -C run clean
