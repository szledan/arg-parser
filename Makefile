all: main

main: build_dir
	cd build/ && make main

build-unit-tests: build_dir
	cd build/ && make unit

run-unit-tests: build-unit-tests
	cd build/ && ./bin/unit

build_dir:
	mkdir -p build/
	cd build/ && cmake ../

clean:
	cd build && make clean

distclean:
	rm -rf build/
