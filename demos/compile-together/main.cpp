/* Copyright (C) 2016, Szilard Ledan <szledan@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "arg-parse.h"
#include <iostream>

int main(int argc, char* argv[])
{
    argparse::ArgParse args;

    args.add(argparse::Flag("--flag", "-f", "This flag is only setable, doesn't have value."));
    args.add(argparse::Flag("--number", "-n", "Set a 'number', this flag does not have 'default value', but has 'name' and 'desciption'.", argparse::Value("", "integer", "An integer number.")));
    args.add(argparse::Flag("--choose", "-c", "Choose a value with 'default'.", argparse::Value("A", {"A", "B", "C"})));
    args.add(argparse::Flag("--pi", /* long flag */
                            "-p", /* short flag */
                            "Set 'pi' value, this flag has 'default value', 'name' and 'desciption'.", /* description for help */
                            argparse::Value("3.14", /* default string of value; if it is not set then value of flag needs to be set. */
                                            "pi", /* name of value of flag, only for help */
                                            "The pi number." /* description of value of flag, only for help */)));
    args.add(argparse::Arg("first", "This arg needs to be set.", argparse::Arg::IsNeeded));
    args.add(argparse::Arg("second", "This arg does not need.", !argparse::Arg::IsNeeded));

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
