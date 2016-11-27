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

#include "test-manual.hpp"

#include "arg-parse.hpp"
#include <iostream>
#include <sstream>
#include <string>

namespace testargparse {
namespace {

using namespace argparse;

TestContext::Return showHelp(TestContext* ctx)
{
    struct {
        const std::string option;
    } testCases[] {
        "help.show=0,help.margin=30",
        "help.show=1,help.tab=\t,help.add=false",
        "help.show=2,program.name=show-all,help.compact=off,help.margin=20",
        ctx->param.str
    };

    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase) {
        const std::string& interlacedOption = testCases[testCase].option;
        const std::string caseName = TAP_CASE_NAME(testCase, interlacedOption);
        std::cout << "--- " << caseName << " ---" << std::endl;

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS("--undefined"), TAP_CHARS("2"), TAP_CHARS("3"), TAP_CHARS("4"), TAP_CHARS("5"), TAP_CHARS("6") };
        const int argc = TAP_ARRAY_SIZE(argv);

        ArgParse ap(interlacedOption);
        ap.def(Flag("", "-s", "Only 'short' flag def, is a setter == without value."));
        ap.def(Flag("--only-long", "", "Only 'long' flag def, is a setter == without value."));
        ap.def(Flag("--long", "-l", "Both 'long' and 'short' flag def, is a setter == without value."));
        ap.def(Flag("", "-o", "Only 'short' flag def, with value.", Value("default", Value::Required)));
        ap.def(Flag("--only-long-with", "", "Only 'long' flag def, with value.", Value("default", !Value::Required)));
        std::stringstream multilineDescription;
        multilineDescription << "Both 'long' and 'short' flag def," << std::endl << "with value" << std::endl << "and uses of multi-line description.";
        ap.def(Flag("--long-with", "-L", multilineDescription.str(), Value("default", Value::Required, "value", "This value is required.")));
        ap.def(Flag("", "-c", "Only 'short' flag def, with 'chooser' value.", Value( { "D", "A", "B", "C", "E" }, Value::Required)));
        ap.def(Flag("--only-long-chooser", "", "Only 'long' flag def, with 'chooser' value.", Value( { "D", "A", "B", "C", "E" }, !Value::Required)));
        ap.def(Flag("--long-chooser", "-C", "Both 'long' and 'short' flag def, with 'chooser' value.", Value( { "D", "A", "B", "C", "E" }, Value::Required, "choose", "This chooser is required.")));

        ap.def(Arg("first"));
        ap.def(Arg("second", "Has description."));
        ap.def(Arg("third", "", Arg::Required));
        ap.def(Arg("fourth", "", Arg::Required, Value("3.14")));
        multilineDescription.str("");
        multilineDescription << "Has description" << std::endl << "with default value" << std::endl << "and uses of multi-line description.";
        ap.def(Arg("fifth", multilineDescription.str(), Arg::Required, Value("last", !Value::Required, "The value description.")));
        ap.def(Arg("sixth", "With chooser value", Arg::Required, Value( { "D", "A", "B", "C", "E" } )));
        ap.def(Arg("seventh", "", !Arg::Required, Value("1")));
        ap.def(Arg("eighth", "Has description with default value", !Arg::Required, Value("true", Value::Required, "The value description.")));
        ap.def(Arg("nineth", "With chooser value", !Arg::Required, Value( { "D", "A", "B", "C", "E" } )));

        if (!ap.parse(argc, argv))
            std::cout << ap.error() << std::endl;
        std::cout << ap.help() << std::endl;
    }

    return TestContext::Return::Pass;
}

} // namespace anonymous

void manualHelpTests(TestContext* ctx)
{
    showHelp(ctx);
}

} // namespace testargparse
