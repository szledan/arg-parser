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

    args.add(argparse::Flag("--unit", "-u", "Select unit tests."));
    args.add(argparse::Flag("--manual", "-m", "Select manual tests.",
                            argparse::Value("program.name=show-help,help.add=true,tab=\t,mode.strict=true,help.compact=on,help.show=2", "options")));
    args.add(argparse::Flag("--silent", "-s", "Fails show only."));

    if (!args.parse(argc, argv)) {
        std::cout << args.error() << std::endl;
        std::cout << args.help() << std::endl;
        return 1;
    }

    if (args["--help"].isSet) {
        std::cout << args.help() << std::endl;
        return 0;
    }

    const bool all = args["--all"].isSet || !(args["-u"].isSet || args["-m"].isSet);

    testargparse::TestContext ctx(!args["--silent"].isSet);

    if (args["--unit"].isSet || all) {
        testargparse::argErrorTests(&ctx);
        testargparse::flagTests(&ctx);
        testargparse::valueTests(&ctx);
        testargparse::parserTests(&ctx);
        testargparse::operatorTests(&ctx);
        testargparse::checkFlagTests(&ctx);
        testargparse::checkFlagAndReadValueTests(&ctx);
        testargparse::countsTests(&ctx);
//        testargparse::optionsTests(&ctx);
//        testargparse::argStructTests(&ctx);
//        testargparse::valueStructTests(&ctx);
//        testargparse::flagStructTests(&ctx);
    }

    if (args["--manual"].isSet || all) {
        ctx.param.str = args["--manual"].value.str;
        testargparse::manualHelpTest(&ctx);
//        testargparse::manualErrorTest(&ctx);
    }

    return ctx.run();
}

// TestContext

// Util functions.
namespace {

#include <math.h>

inline float perCent(const size_t& counter, const size_t& denom, const float& precision = 100.0f)
{
    return (denom * precision) ? trunc(float(counter) / float(denom) * precision * 100.0f) / precision : 0.0f;
}

} // namespace anonymous

void testargparse::TestContext::add(testargparse::TestContext::TestInstanceFunc test)
{
    _tests.push_back(test);
}

int testargparse::TestContext::run()
{
    const size_t nums = _tests.size();
    size_t pass = 0;

    if (nums) {
        _result << std::endl << "Run " << nums << " collected test(s)!" << std::endl;
        if (!_showPass)
            _result << "Fails show only." << std::endl;
        _result << std::endl;

        for (auto test : _tests)
            if (test(this))
                pass++;

        _result << std::endl << "Results:" << std::endl;
        _result << "  Pass: " << pass << "/" << nums << " (" << perCent(pass, nums) << "%)" << std::endl;
        _result << "  Fail: " << nums - pass << "/" << nums << " (" << perCent(nums - pass, nums) << "%)" << std::endl;

        std::clog << _result.str();
    }

    return pass == nums ? 0 : 1;
}

bool testargparse::TestContext::pass(const std::string& msg, const std::string& file, const std::string& func, const std::string& line)
{
    if (_showPass) {
        this->test(file, func, line);
        _result << "\033[32;1m" << "PASS" << "\033[39m\033[22m\033[49m: " << msg << std::endl;
    }
    return true;
}

bool testargparse::TestContext::fail(const std::string& msg, const std::string& file, const std::string& func, const std::string& line)
{
    this->test(file, func, line);
    _result << "\033[31;1m" << "FAIL" << "\033[39m\033[22m\033[49m: " << msg << std::endl;
    return false;
}

inline void testargparse::TestContext::test(const std::string& file, const std::string& func, const std::string& line)
{
    _result << "The " << func  << "() at " << file << ":" << line << std::endl;
}
