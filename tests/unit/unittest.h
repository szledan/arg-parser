#ifndef UNITTEST_H
#define UNITTEST_H

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
#include <string>
#include <sstream>
#include <vector>

namespace unittest {

#ifndef UT_ARRAY_SIZE
#define UT_ARRAY_SIZE(A) (sizeof(A) ? sizeof(A) / sizeof(A[0]) : 0)
#endif // UT_ARRAY_SIZE

#ifndef UT_FILE_FUNC_LINE
#define UT_FILE_FUNC_LINE std::string(__FILE__), std::string(__func__), std::to_string(__LINE__)
#endif // UT_FILE_FUNC_LINE

#ifndef UT_PASS
#define UT_PASS(CTX, MSG) CTX->pass(MSG, UT_FILE_FUNC_LINE)
#endif // UT_PASS

#ifndef UT_FAIL
#define UT_FAIL(CTX, MSG) CTX->fail(MSG, UT_FILE_FUNC_LINE)
#endif // UT_FAIL

class TestContext {
public:
    TestContext() : _pass(0), _nums(0) {}
    typedef bool (*TestInstanceFunc)(TestContext*);

    void add(TestInstanceFunc);
    int run();

    bool pass(const std::string& msg, const std::string& file, const std::string& func, const std::string& line);
    bool fail(const std::string& msg, const std::string& file, const std::string& func, const std::string& line);
private:
    void test(const std::string& file, const std::string& func, const std::string& line);

    uint32_t _pass;
    uint32_t _nums;
    std::vector<TestInstanceFunc> _tests;
    std::stringstream _result;
};

void argErrorTests(TestContext*);
void flagTests(TestContext*);
void valueTests(TestContext*);
void manualHelpTest(TestContext*);

#ifndef UT_CHECK_NON_REQUIRED_ERRORS
#define UT_CHECK_NON_REQUIRED_ERRORS(CTX, ARGS, NUMS) do { \
        if (args.errors().size() != NUMS) { \
            std::string msg("Generated non required error(s)! Number Of required errors: "); \
            msg += std::to_string(NUMS) + ", but have: "; \
            msg += std::to_string(args.errors().size()) + ". "; \
            msg += "Error message(s): "; \
            for (auto const& err : args.errors()) \
                msg += err.errorMessage + " "; \
            return UT_FAIL(ctx, msg);  \
        } \
    } while (false)

#endif // UT_CHECK_NON_REQUIRED_ERRORS

#ifndef UT_CHECK_PARSER_EXPECTED_RETURN
#define UT_CHECK_PARSER_EXPECTED_RETURN(CTX, CONDITION) do { \
        if (CONDITION) { \
            return UT_FAIL(ctx, "The parse() returns wrong value! The wrong condition is: " #CONDITION "."); \
        } \
    } while (false)

#endif // UT_NON_REQUIRED_ERRORS

} // namespace unittest

#endif // UNITTEST_H
