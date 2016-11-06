all: main

copy-arg-parse:
	cp ../../src/arg-parse.* .

build: build-dir
	g++ -Wall -pedantic -std=c++11 -o ./build/test test.cpp arg-parse.cpp param-parse.cpp

test: copy-arg-parse build

build-dir:
	mkdir -p build/

clean:
	rm -rf build/
