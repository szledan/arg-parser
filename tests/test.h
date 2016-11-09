#ifndef TEST_H
#define TEST_H

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
#include "test-defs.h"
#include <string>
#include <sstream>
#include <vector>

namespace testargparse {

class TestContext {
public:
    TestContext(const bool& showPass = true) : _pass(0), _nums(0), _showPass(showPass) {}
    typedef bool (*TestInstanceFunc)(TestContext*);

    void add(TestInstanceFunc);
    int run();

    bool pass(const std::string& msg, const std::string& file, const std::string& func, const std::string& line);
    bool fail(const std::string& msg, const std::string& file, const std::string& func, const std::string& line);

    struct Param {
        std::string str;
    } param;

private:
    void test(const std::string& file, const std::string& func, const std::string& line);

    uint32_t _pass;
    uint32_t _nums;
    const bool _showPass;
    std::vector<TestInstanceFunc> _tests;
    std::stringstream _result;
};

// Unit and api tests.
void argErrorTests(TestContext*);
void flagTests(TestContext*);
void valueTests(TestContext*);
void parserTests(TestContext*);
void operatorTests(TestContext*);
void checkFlagTests(TestContext*);
void checkFlagAndReadValueTests(TestContext*);
void countsTests(TestContext*);

// Manual tests.
void manualHelpTest(TestContext*);

} // namespace testargparse

#endif // TEST_H
