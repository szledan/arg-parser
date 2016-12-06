all: build.demos

build.tests: make.dir
	cd build/ && make tests

build.demos: make.dir
	cd build/ && make demos

make.dir:
	mkdir -p build/
	cmake -H. -Bbuild

clean:
	cd build && make clean

dist-clean:
	rm -rf build/
