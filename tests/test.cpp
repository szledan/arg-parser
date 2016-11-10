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

#include <iostream>

namespace testargparse {

// Util functions.
namespace {

#include <math.h>

inline float perCent(const size_t& counter, const size_t& denom, const float& precision = 100.0f)
{
    return (denom * precision) ? trunc(float(counter) / float(denom) * precision * 100.0f) / precision : 0.0f;
}

} // namespace anonymous

// TestContext

TestContext::TestContext(const bool& showPass)
    : _showPass(showPass)
{
    _result << std::endl << "ArgParse Test Suite created." << std::endl;
    _result << std::endl << "Ready to collecting tests." << std::endl;
}

void TestContext::add(TestContext::TestInstanceFunc test)
{
    _tests.insert(test);
}

int TestContext::run()
{
    const size_t nums = _tests.size();
    size_t pass = 0;
    size_t nott = 0;

    if (nums) {
        _result << std::endl << "Run " << nums << " collected test(s)!" << std::endl;
        if (!_showPass)
            _result << "Passes does not show." << std::endl;
        _result << std::endl;

        for (auto test : _tests) {
            switch (test(this)) {
            case Return::Pass: pass++; break;
            case Return::NotTested: nott++; break;
            default: break;
            }
        }

        _result << std::endl << "Results:" << std::endl;
        _result << "  Pass: " << pass << "/" << nums << " (" << perCent(pass, nums) << "%)" << std::endl;
        _result << "  Fail: " << nums - (pass + nott) << "/" << nums << " (" << perCent(nums - (pass + nott), nums) << "%)" << std::endl;
        _result << "  Not tested: " << nott << "/" << nums << " (" << perCent(nott, nums) << "%)" << std::endl;

        std::clog << _result.str();
    }

    return pass == nums ? 0 : 1;
}

TestContext::Return TestContext::pass(const std::string& msg, const std::string& file, const std::string& func, const std::string& line)
{
    if (_showPass) {
        this->test(file, func, line);
        _result << "\033[32;1m" << "  PASS" << "\033[39m\033[22m\033[49m: " << msg << std::endl;
    }
    return Return::Pass;
}

TestContext::Return TestContext::fail(const std::string& msg, const std::string& file, const std::string& func, const std::string& line)
{
    this->test(file, func, line);
    _result << "\033[31;1m" << "  FAIL" << "\033[39m\033[22m\033[49m: " << msg << std::endl;
    return Return::Fail;
}

TestContext::Return TestContext::nott(const std::string& msg, const std::string& file, const std::string& func, const std::string& line)
{
    this->test(file, func, line);
    _result << "\033[33;1m" << "  NOT TESTED" << "\033[39m\033[22m\033[49m: " << msg << std::endl;
    return Return::NotTested;
}

inline void TestContext::test(const std::string& file, const std::string& func, const std::string& line)
{
    _result << "The " << func  << "() at " << file << ":" << line << std::endl;
}

} // namespace testargparse
