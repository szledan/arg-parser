all: main

copy-arg-parse:
	cp ../../src/arg-parse.* .

main: copy-arg-parse build-dir
	g++ -Wall -pedantic -std=c++11 -o ./build/main main.cpp arg-parse.cpp

build-dir:
	mkdir -p build/

clean:
	rm -rf build/
