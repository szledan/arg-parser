
ARG-PARSER
=====

The ArgParser aims to be a simple argument parser to `c++11`.

## Getting Started

Easy to use: download the header, and include it. :)

## Usage

### Creations

```c++
// Use by default options.
ap::ArgParser ap;

ap::ArgParser ap();

```

### Arg definitions

```c++
// Define Arg.
ap::Arg arg = ap::Arg("name").defRequired()
                             .defNonRequired()
                             .defDefault("default value")
                             .defHelp("Brief help. \nOr multi-line longer.")
                             .defChooseList("chooseA|ChooseB|ChooseC");

// Simple Arg without name or default value.
ap.def(ap::Arg());

// Arg with name.
ap.def(ap::Arg("file"));

// Arg with default value.
ap.def(ap::Arg().defDefault("/home/user"));

// Arg is required. (This is the default construction.)
ap.def(ap::Arg().defRequired());

// Arg is non-required. (This is the default construction.)
ap.def(ap::Arg().defNonRequired());

// Arg with help message.
ap.def(ap::Arg().defHelp("The path of the input file."));

// Arg with choose list.
ap.def(ap::Arg().defChooseList("first|second|other"));
// If you didn't set the default value, then the 'first' choose item will be that.

```

### Flag definitions

```c++
// Define Flag.
ap::Flag flag = ap::Flag("name", "alias").defAlias("more alias")
                                         .defHelp("Brief help. \nOr multi-line longer.")
                                         .defArg(arg);

// Simple flag.
ap.def(ap::Flag("--flag"));

// Flag with one alias.
ap.def(ap::Flag("--out", "-o"));

// Flag with more aliases.
ap.def(ap::Flag("--help", "-h").defAlias("-?").defAlias("--usage"));

// Flag with help message.
ap.def(ap::Flag("--pi").defHelp("Get the pi number.");

// Flag with arguments.
ap.def(ap::Flag("--pi").defArg(ap::Arg(...));
```

### Simple usage

```c++
// A minimal 'copy' command.
int main (int argc, char* argv[])
{
    ap::ArgParser ap;
    ap.def(ap::Arg("source").defHelp("Copy source file(s) to destination."));
    ap.def(ap::Arg("dest").defHelp("The destination file/dir. Rewrites the existed dest!"));
    ap.def(ap::Flag("--recursive", "-R").defAlias("-r").defHelp("Copy directories recursively."));
    ap.def(ap::Flag("--sparse").defHelp("Control creation of sparse files.")
                               .defArg(ap::Arg("when").defChooseList("auto|always|never")));

    if (!ap.parse(argc, argv))
    {
        std::cout << ap.help() << std::endl;
        return 1;
    }

    std::atring programName = ap[0];
    std::atring source = ap[1];
    std::atring dest = ap[2];

    bool isRecursive = ap["-r"].isSet;
    enum Sparse {
        AUTO = 0,
        ALWAYS,
        NEVER
    } sparse = ap["--sparse"].read<int>();

    return 0;
}
```


## For developers

### Build & run tests

Check out the source code
```
git clone https://github.com/szledan/arg-parse.git
```
Build all tests
```
make build-tests
```
Run all tests
```
./build/bin/tests
```
