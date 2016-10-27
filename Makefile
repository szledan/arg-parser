all: unittest main

unittest: makedir
	g++ -std=c++11 -o ./bin/unittest src/arg-parse.cpp test/unittest.cpp

main: makedir
	g++ -std=c++11 -o ./bin/main src/arg-parse.cpp src/main.cpp

makedir:
	mkdir -p ./bin

clean:
	rm -rf ./bin
