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

#include "test-unit.h"

#include "arg-parse.h"
#include <assert.h>

namespace testargparse {
namespace {

using namespace argparse;

void testCallBackFunc(void)
{
    assert(false);
}

TestContext::Return testDefFlag(TestContext* ctx)
{
    const std::string longFlagStr = "--long";
    const std::string shortFlagStr = "-s";
    const std::string valueStr = "value";

    struct {
        CallBackFunc func;
    } callBackFuncCases[] = {
        { nullptr },
        { testCallBackFunc },
    };

    struct {
        const Flag flag;
        struct {
            const bool valid;
            const bool hasLong;
            const bool hasShort;
        } const expected;
    } testCases[] = {
        { Flag(longFlagStr, ""), { true, true, false } },
        { Flag("", shortFlagStr), { true, false, true } },
        { Flag(longFlagStr, shortFlagStr), { true, true, true } },
        { Flag(longFlagStr + "=" + valueStr, ""), { true, true, false } },
        { Flag(longFlagStr + "=" + valueStr, shortFlagStr), { true, true, true } },
        { Flag(longFlagStr + " " + valueStr, ""), { true, true, false } },
        // Not valid testcases.
        { Flag(), { false, false, false } },
        { Flag("", ""), { false, false, false } },
        { Flag(" ", " "), { false, true, true } },
        { Flag("", longFlagStr), { false, false, true } },
        { Flag(shortFlagStr, ""), { false, true, false } },
        { Flag(shortFlagStr, longFlagStr), { false, true, true } },
        { Flag("", "-s "), { false, false, true } },
        { Flag("", " -s"), { false, false, true } },
        { Flag(" --long", "s"), { false, true, true } },
        { Flag("-abc", "ab"), { false, true, true } },
        { Flag("abc", "s "), { false, true, true } },
    };

    for (size_t callBackFuncCase = 0; callBackFuncCase < TAP_ARRAY_SIZE(callBackFuncCases); ++callBackFuncCase)
    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase) {
        const CallBackFunc callBackFunc = callBackFuncCases[callBackFuncCase].func;
        const Flag defFlag = testCases[testCase].flag;
        const std::string caseName = std::to_string(testCase) + "|"
                + "Flag(" + defFlag._longFlag + "," + defFlag._shortFlag + ")"
                + " testcase. ";

        ArgParse ap;
        const Flag& addedFlag = ap.def(defFlag, callBackFunc);

        const std::string validStr = std::string("valid=") + std::to_string(testCases[testCase].expected.valid);

        if (!testCases[testCase].expected.valid) {
            if (TAP_CHECK(ctx, &addedFlag != &Flag::WrongFlag))
                return TAP_FAIL(ctx, caseName + "Expected:" + validStr);
            continue;
        };

        assert(testCases[testCase].expected.valid);

        if (TAP_CHECK(ctx, &addedFlag == &Flag::WrongFlag))
            return TAP_FAIL(ctx, caseName + "Expected:" + validStr);

        if (TAP_CHECK(ctx, addedFlag._longFlag.empty() && addedFlag._shortFlag.empty()))
            return TAP_FAIL(ctx, caseName + "!!!");

        if (!addedFlag._longFlag.empty()) {
            if (TAP_CHECK(ctx, &(ap[addedFlag._longFlag]) != &addedFlag))
                return TAP_FAIL(ctx, caseName + "!!!");

            if (TAP_CHECK(ctx, ap[addedFlag._longFlag]._callBackFunc != callBackFunc))
                return TAP_FAIL(ctx, caseName + "!!!");
        }

        if (!addedFlag._shortFlag.empty()) {
            if (TAP_CHECK(ctx, &(ap[addedFlag._shortFlag]) != &addedFlag))
                return TAP_FAIL(ctx, caseName + "!!!");

            if (TAP_CHECK(ctx, ap[addedFlag._shortFlag]._callBackFunc != callBackFunc))
                return TAP_FAIL(ctx, caseName + "!!!");
        }
    }

    return TAP_PASS(ctx, "The ArgParse::def(Flag(LONG, SHORT), C_B_FUNC) test.");
}

TestContext::Return testDefArg(TestContext* ctx)
{
    TAP_VALUE_STR_TEST_CASES(nameCases);
    TAP_REQUIRED_TEST_CASES(requiredCases);
    TAP_VALUE_STR_TEST_CASES(valueStrCases);

    for (size_t nameCase = 0; nameCase < TAP_ARRAY_SIZE(nameCases); ++nameCase)
    for (size_t requiredCase = 0; requiredCase < TAP_ARRAY_SIZE(requiredCases); ++requiredCase)
    for (size_t valueStrCase = 0; valueStrCase < TAP_ARRAY_SIZE(valueStrCases); ++valueStrCase) {
        const bool required = requiredCases[requiredCase].required;
        const std:: string testStr = valueStrCases[valueStrCase].str;
        const std:: string testValueName = "value-name";
        const std:: string testValueDescription = "value-description";

        TAP_VALUE_TEST_CASES(valueCases, testStr, required, testValueName, testValueDescription);

        for (size_t valueCase = 0; valueCase < TAP_ARRAY_SIZE(valueCases); ++valueCase) {
            const Value value = valueCases[valueCase].value;
            const std::string testName = nameCases[nameCase].str;
            const std::string testDescription = "description";

            TAP_ARG_TEST_CASES(testCases, testName, testDescription, required, value);

            for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase) {
                const Arg& arg  = testCases[testCase].arg;
                const std::string caseName = std::to_string(testCase) + ".(test-name:" + arg._name + ",value-case:" + std::to_string(valueCase) + ") testcase. ";

                ArgParse ap;
                const Arg& addedArg = ap.def(arg);

                if (!arg._name.empty()) {
                    if (TAP_CHECK(ctx, !addedArg.isDefined))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, &addedArg == &Arg::WrongArg))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, &(ap[0]) == &Arg::WrongArg))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, addedArg._name.empty()))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, addedArg.isRequired != arg.isRequired))
                        return TAP_FAIL(ctx, caseName + "!!!");
                } else {
                    assert(arg._name.empty());
                    if (TAP_CHECK(ctx, addedArg.isDefined))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, &addedArg != &Arg::WrongArg))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, &(ap[0]) != &Arg::WrongArg))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, !addedArg._name.empty()))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, addedArg.isRequired))
                        return TAP_FAIL(ctx, caseName + "!!!");
                }
            }
        }
    }

    return TAP_PASS(ctx, "The ArgParse::def(Arg(NAME, '', REQUIRE, VALUE)) test.");
}

} // namespace anonymous

void unitDefTests(TestContext* ctx)
{
    ctx->add(testDefFlag);
    ctx->add(testDefArg);
}

} // namespace testargparse
