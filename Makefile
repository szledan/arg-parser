all: arg-parse

arg-parse: build-dir
	cd build/ && make arg-parse

tests.build: build-dir
	cd build/ && make tests

tests.run.unit: tests.build
	cd build/ && ./bin/tests --unit

tests.run.manual: tests.build
	cd build/ && ./bin/tests --manual

tests.run.all: tests.build
	cd build/ && ./bin/tests

build-dir:
	mkdir -p build/
	cd build/ && cmake ../

clean:
	cd build && make clean

dist-clean:
	rm -rf build/
