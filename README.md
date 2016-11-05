
ARG-PARSE
=====

The ArgParse aims to be a simple argument parser to `c++11`.

## Getting Started

Check out the source code
```
git clone https://github.com/szledan/arg-parse.git
```
Build `arg-parse` dynamic library
```
make arg-parse
```
## Usage

Read more: [API reference](./doc/api-reference.md) and [examples](./demos/README.md).

Source files
```
./arg-parse.cpp
./arg-parse.h
./main.cpp
```

A simple `main.cpp`
```c++
#include "arg-parse.h"
#include <iostream>

int main(int argc, char* argv[])
{
    argparse::ArgParse args;
    
    args.add(argparse::Arg("first", "The first arg is needed.", argparse::Arg::IsNeeded));
    args.add(argparse::Arg("second", "The second arg is not needed.", !argparse::Arg::IsNeeded));
    args.add(argparse::Flag("--flag", "-f", "This flag is only setable, has not value."));
    args.add(argparse::Flag("--pi", "-p", "Set 'pi' value, this flag has 'default value', 'name' and 'desciption'.", Value("3.14", "pi", "The pi number.")));
    args.add(argparse::Flag("--number", "-n", "Set a 'number', this flag has not 'default value', but has 'name' and 'desciption'.", Value("", "integer", "An integer number.")));
    args.add(argparse::Flag("--choose", "-c", "Choose a value with 'default'.", Value("A", {"A", "B", "C"})));
    
    if (!args.parse(argc, argv)) {
        // Show error messages.
        std::cout << args.error() << std::endl;
        // Show help.
        std::cout << args.help() << std::endl;
        return 1;
    }
    
    const argparse::Arg& first = args[0];
    if (first.isSet) {
        std::cout << "This first is set, value is: " << first.str << std::endl;
    }
    
    const argparse::Flag& pi = args["--pi"];
    if (pi.isSet) {
        std::cout << "The 'pi' is set, the new value is: " << pi.value.str << std::endl;
    } else {
        std::cout << "The 'pi' is not set, the old value is: " << pi.value.str << std::endl;
    }

    return 0;
}
```

Build
```
mkdir -p build && g++ -std=c++11 -o ./build/main main.cpp arg-parse.cpp
```

Run: `./build/main`.

## For developers

### Build & run tests

Build all tests
```
make build-tests
```
Run all tests
```
make run-tests
```
