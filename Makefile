all: arg-parse

arg-parse: build-dir
	cd build/ && make arg-parse

build-tests: build-dir
	cd build/ && make tests

build-dir:
	mkdir -p build/
	cd build/ && cmake ../

clean:
	cd build && make clean

dist-clean:
	rm -rf build/
