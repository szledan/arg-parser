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

#include "test.h"

#include "arg-parse.h"
#include <iostream>

// Configure and run tests.

int main(int argc, char* argv[])
{
    argparse::ArgParse args;

    {
        // Define options.
        args.def(argparse::Flag("--api", "-a", "Select api tests."));
        args.def(argparse::Flag("--manual", "-m", "Select manual tests.",
                                argparse::Value("program.name=show-help,help.add=true,tab=\t,mode.strict=true,help.compact=on,help.show=2", "options")));
        args.def(argparse::Flag("--unit", "-u", "Select unit tests."));
        args.def(argparse::Flag("--silent", "-s", "Fails show only."));

        // Parse argv and argc and check errors.
        if (!args.parse(argc, argv)) {
            std::cout << args.error() << std::endl;
            std::cout << args.help() << std::endl;
            return 1;
        }

        // Check help flags.
        if (args["--help"].isSet) {
            std::cout << args.help() << std::endl;
            return 0;
        }
    }

    // Collect running options.
    const bool none = !(args["-a"].isSet || args["-m"].isSet || args["-u"].isSet);
    const bool all = args["--all"].isSet || none;

    testargparse::TestContext ctx(!args["--silent"].isSet);

    // Run manual tests.
    if (args["--manual"].isSet || all) {
        ctx.param.str = args["--manual"].value.str;
        testargparse::manualTests(&ctx);
    }

    // Collect api tests.
    if (args["--api"].isSet || all) {
        testargparse::apiTests(&ctx);
    }

    // Collect unit and behavior tests.
    if (args["--unit"].isSet || all) {
        testargparse::unitAndBehaviorTests(&ctx);
    }

    // Run collected tests.
    int ret = ctx.run();

    return ret;
}
