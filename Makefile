all: arg-parse

arg-parse: build_dir
	cd build/ && make arg-parse

build-tests: build_dir
	cd build/ && make unit

run-unit-tests: build-tests
	cd build/ && ./bin/unit --unit

run-manual-tests: build-tests
	cd build/ && ./bin/unit --manual

run-tests: build-tests
	cd build/ && ./bin/unit

build_dir:
	mkdir -p build/
	cd build/ && cmake ../

clean:
	cd build && make clean

distclean:
	rm -rf build/
