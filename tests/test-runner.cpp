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

#include "test.hpp"

#include "arg-parse.hpp"
#include <iostream>

// Configure and run tests.

int main(int argc, char* argv[])
{
    struct {
        const bool nonSpecified() const { return !api && !unit && ! manual; }
        bool all = false;
        bool api = false;
        bool silent = false;
        bool unit = false;
        bool manual = false;
        std::string manualValue = "";
    } options;

    // Parse arguments.
    {
        using namespace argparse;
        ArgParse args;

        // Define options.
        args.def(Flag("--api", "-a", "Select api tests."));
        Value helpTestConfigs = Value("program.name=show-help"
                                      ",mode.strict=true"
                                      ",help.add=true"
                                      ",help.tab=\t"
                                      ",help.compact=on"
                                      ",help.margin=26"
                                      ",help.show=2" /*< Default value. */,
                                      !Value::Required, /*< Set non required. */
                                      "options" /*< Name of value. */,
                                      "This value is interlaced, see more: ArgParse Api Reference." /*< Description of value. */);
        args.def(Flag("--manual", "-m", "Select manual tests.", helpTestConfigs));
        args.def(Flag("--unit", "-u", "Select unit tests."));
        args.def(Flag("--silent", "-s", "Fails show only."));

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

        // Collect options.
        options.all = args["--all"].isSet;
        options.api = args["--api"].isSet;
        options.silent = args["--silent"].isSet;
        options.unit = args["--unit"].isSet;
        if ((options.manual = args["--manual"].isSet))
            options.manualValue = args["--manual"].value.str;
    }

    // Create test context.
    testargparse::TestContext ctx(!options.silent);

    const bool all = options.all || options.nonSpecified();

    // Run manual tests.
    if (options.manual || all) {
        ctx.param.str = options.manualValue;
        testargparse::manualTests(&ctx);
    }

    // Collect api tests.
    if (options.api || all) {
        testargparse::apiTests(&ctx);
    }

    // Collect unit and behavior tests.
    if (options.unit || all) {
        testargparse::unitAndBehaviorTests(&ctx);
    }

    // Run collected tests.
    int ret = ctx.run();

    return ret;
}
