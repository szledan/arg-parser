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

#include "unittest.h"

#include "arg-parse.h"
#include <iostream>
#include <math.h>

int main(int argc, char* argv[])
{
    argparse::ArgParse args;

    args.add(argparse::Flag("--unit", "-u", "Select unit tests."));
    args.add(argparse::Flag("--manual", "-m", "Select manual tests."));

    if (!args.parse(argc, argv)) {
        std::cout << args.error() << std::endl;
        std::cout << args.help() << std::endl;
        return 1;
    }

    if (args["--help"].isSet) {
        std::cout << args.help() << std::endl;
        return 0;
    }

    const bool all = args["--all"].isSet || !args.defFlagsCount();

    unittest::TestContext ctx;

    if (args["--unit"].isSet || all) {
        unittest::argErrorTests(&ctx);
        unittest::flagTests(&ctx);
        unittest::valueTests(&ctx);
    }

    if (args["--manual"].isSet || all) {
        unittest::manualHelpTest(&ctx);
    }

    return ctx.run();
}

void unittest::TestContext::add(unittest::TestContext::TestInstanceFunc test)
{
    _tests.push_back(test);
}

int unittest::TestContext::run()
{
    const size_t nums = _tests.size();
    size_t pass = 0;

    if (nums) {
        _result << std::endl << "Run " << nums << " collected test(s)!" << std::endl << std::endl;

        for (auto test : _tests)
            if (test(this))
                pass++;

        _result << std::endl << "Results:" << std::endl;
        _result << "  Pass: " << pass << "/" << nums << " (" << (trunc(float(pass) / float(nums) * 10000.0f) / 100.0f) << "%)" << std::endl;
        _result << "  Fail: " << nums - pass << "/" << nums << " (" << (trunc(float(nums - pass) / float(nums) * 10000.0f) / 100.0f) << "%)" << std::endl;

        std::clog << _result.str();
    }

    return pass == nums ? 0 : 1;
}

bool unittest::TestContext::pass(const std::string& msg, const std::string& file, const std::string& func, const std::string& line)
{
    this->test(file, func, line);
    _result << "\033[32;1m" << "PASS" << "\033[39m\033[22m\033[49m: " << msg << std::endl;
    return true;
}

bool unittest::TestContext::fail(const std::string& msg, const std::string& file, const std::string& func, const std::string& line)
{
    this->test(file, func, line);
    _result << "\033[31;1m" << "FAIL" << "\033[39m\033[22m\033[49m: " << msg << std::endl;
    return false;
}

inline void unittest::TestContext::test(const std::string& file, const std::string& func, const std::string& line)
{
    _result << "The " << func  << "() at " << file << ":" << line << std::endl;
}
