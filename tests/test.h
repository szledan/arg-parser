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

#include "test-defs.h"

#include <string>
#include <sstream>
#include <set>

namespace testargparse {

class TestContext {
public:
    enum Return { Fail, Pass, NotTested };
    typedef Return (*TestInstanceFunc)(TestContext*);

    TestContext(const bool& = true);

    void add(TestInstanceFunc);
    int run();

    Return pass(const std::string& msg, const std::string& file, const std::string& func, const std::string& line);
    Return fail(const std::string& msg, const std::string& file, const std::string& func, const std::string& line);
    Return nott(const std::string& msg, const std::string& file, const std::string& func, const std::string& line);

    struct Param {
        std::string str;
    } param;

private:
    void test(const std::string& file, const std::string& func, const std::string& line);

    const bool _showPass;
    std::set<TestInstanceFunc> _tests;
    std::stringstream _result;
};

// Api tests.
void apiTests(TestContext*);

// Manual tests.
void manualTests(TestContext* ctx);

// Unit tests.
void unitAndBehaviorTests(TestContext*);

} // namespace testargparse

#endif // TEST_H
