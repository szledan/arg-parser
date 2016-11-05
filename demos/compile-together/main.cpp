#include "arg-parse.h"
#include <iostream>

int main(int argc, char* argv[])
{
    argparse::ArgParse args;

    args.add(argparse::Arg("first", "This arg needs to be set.", argparse::Arg::IsNeeded));
    args.add(argparse::Arg("second", "This arg does not need.", !argparse::Arg::IsNeeded));
    args.add(argparse::Flag("--flag", "-f", "This flag is only setable, doesn't have value."));
    args.add(argparse::Flag("--number", "-n", "Set a 'number', this flag does not have 'default value', but has 'name' and 'desciption'.", argparse::Value("", "integer", "An integer number.")));
    args.add(argparse::Flag("--choose", "-c", "Choose a value with 'default'.", argparse::Value("A", {"A", "B", "C"})));
    args.add(argparse::Flag("--pi", /* long flag */
                            "-p", /* short flag */
                            "Set 'pi' value, this flag has 'default value', 'name' and 'desciption'.", /* description for help */
                            argparse::Value("3.14", /* default string of value; if it is not set then value of flag needs to be set. */
                                            "pi", /* name of value of flag, only for help */
                                            "The pi number." /* description of value of flag, only for help */)));

    if (!args.parse(argc, argv)) {
        // Show error messages.
        std::cout << args.error() << std::endl;
        // Show help.
        std::cout << args.help() << std::endl;
        return 1;
    }

    const argparse::Arg& first = args[0];
    if (first.isSet) {
        std::cout << "This first arg was set, value is: " << first.str << std::endl;
    }

    const argparse::Flag& pi = args["--pi"];
    if (pi.isSet) {
        std::cout << "The 'pi' was set, the new value is: " << pi.value.str << std::endl;
    } else {
        std::cout << "The 'pi' was not set, the old value is: " << pi.value.str << std::endl;
    }

    return 0;
}