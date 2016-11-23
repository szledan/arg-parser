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

#include "test-unit.hpp"

#include "arg-parse.hpp"

#include <assert.h>

namespace testargparse {
namespace {

using namespace argparse;

const std::string g_shortFlag = "-a";
const std::string g_longFlag = "--a";
const std::string g_description = "Simple settable flag with or without value.";

TestContext::Return testSomeTypicalFlags(TestContext* ctx)
{
    const std::string value = "--v=.V--";

    struct {
        const std::string recievedFlag;
        const std::string definedFlag;
        const std::string definedValue;
        const bool hasValue;
    } testCases[] = {
        { g_longFlag, g_longFlag, "", false },  // Defined without value.
        { g_shortFlag, g_shortFlag, "", false },  // Defined without value.
        { g_longFlag, g_longFlag, value, true },  // Defined with value.
        { g_shortFlag, g_shortFlag, value, true },  // Defined with value.
        { g_longFlag + "=" + value, g_longFlag, "", true },  // Defined with value joined to flag with '='.
        { g_shortFlag + "=" + value, g_shortFlag, "", false },  // Defined with value joined to flag with '='.
    };

    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase)
    {
        const std::string recievedFlag = testCases[testCase].recievedFlag;
        const std::string definedFlag = testCases[testCase].definedFlag;
        const std::string definedValue = testCases[testCase].definedValue;
        const std::string caseName = recievedFlag + "|" + definedFlag + " testcase. ";

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(recievedFlag.c_str()), TAP_CHARS(definedValue.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        ArgParse ap;

        ap.def(Flag("", g_shortFlag, g_description, Value("", "value")));
        ap.def(Flag(g_longFlag, "", g_description, Value("", "value")));
        ap.def(Flag("--value", "-v", "Flag with value.", Value("default", "value")));

        const bool parseRet = ap.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, ap, 0);

        if (!ap[definedFlag].isSet)
            return TAP_FAIL(ctx, caseName + "The flag is not set!");

        if (testCases[testCase].hasValue) {
            if (!ap[definedFlag].hasValue)
                return TAP_FAIL(ctx, caseName + "Wrong set of 'hasValue'!");

            if (!ap[definedFlag].value.isRequired)
                return TAP_FAIL(ctx, caseName + "Wrong 'isRequired'!");

            if (ap[definedFlag].value.str != value)
                return TAP_FAIL(ctx, caseName + "Different values!");
        }
    }

    return TAP_PASS(ctx, "Parse some typical flags.");
}

TestContext::Return testUndefinedFlags(TestContext* ctx)
{
    const std::string value = "--v=.V--";

    struct {
        const std::string recievedFlag;
        const std::string definedFlag;
        const std::string definedValue;
        const bool hasValue;
    } testCases[] = {
        { g_longFlag, g_longFlag, "", false },  // Undefined without value.
        { g_shortFlag, g_shortFlag, "", false },  // Undefined without value.
        { g_longFlag + "=" + value, g_longFlag, "", true },  // Undefined with value joined to flag with '='.
        { g_shortFlag + "=" + value, g_shortFlag, "", false },  // Undefined with value joined to flag with '=', but is not a value, it is a shortFlags type.
    };

    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase)
    {
        const std::string recievedFlag = testCases[testCase].recievedFlag;
        const std::string definedFlag = testCases[testCase].definedFlag;
        const std::string definedValue = testCases[testCase].definedValue;
        const std::string caseName = recievedFlag + "|" + definedFlag + " testcase. ";

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(recievedFlag.c_str()), TAP_CHARS(definedValue.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        ArgParse ap;

        const bool parseRet = ap.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, ap, 0);

        if (!ap[definedFlag].isSet)
            return TAP_FAIL(ctx, caseName + "The flag is not set!");

        if (testCases[testCase].hasValue) {
            if (!ap[definedFlag].hasValue)
                return TAP_FAIL(ctx, caseName + "Wrong set of 'hasValue'!");

            if (ap[definedFlag].value.isRequired)
                return TAP_FAIL(ctx, caseName + "Wrong 'isRequired'!");

            if (ap[definedFlag].value.str != value)
                return TAP_FAIL(ctx, caseName + "Different values!");
        }
    }

    return TAP_PASS(ctx, "Parse undefined flags.");
}

TestContext::Return testNonTypicalFlags(TestContext* ctx)
{
    struct {
        const std::string recievedFlag;
        const std::string definedFlag;
        const std::string definedValue;
        const bool hasValue;
    } testCases[] = {
        // Undefined test cases.
        { "--a=v", "--a", "v", true },
        { "--a=", "--a", "", true },
        { "--a-a", "--a-a", "", false },
        // Defined test cases.
        { "--b=", "--b=", "", false },
        { "--b=v", "--b=v", "", false },
        { "--c=", "--c=", "v", true },
        { "--c=v", "--c=v", "v", true },
        { "--d=v", "--d", "v", true },
        { "--e", "--e", "", false },
    };

    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase)
    {
        const std::string recievedFlag = testCases[testCase].recievedFlag;
        const std::string definedFlag = testCases[testCase].definedFlag;
        const std::string definedValue = testCases[testCase].definedValue;
        const std::string caseName = recievedFlag + "|" + definedFlag + " testcase. ";

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(recievedFlag.c_str()), TAP_CHARS(definedValue.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        ArgParse ap;

        ap.def(Flag("--b=", "", g_description));
        ap.def(Flag("--b=v", "", g_description));
        ap.def(Flag("--c=", "", g_description, Value("v")));
        ap.def(Flag("--c=v", "", g_description, Value("v")));
        ap.def(Flag("--d", "", g_description, Value("v")));
        ap.def(Flag("--e", "", g_description));

        const bool parseRet = ap.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, ap, 0);

        if (!ap[definedFlag].isSet)
            return TAP_FAIL(ctx, caseName + "The flag is not set!");

        if (testCases[testCase].hasValue) {
            if (!ap[definedFlag].hasValue)
                return TAP_FAIL(ctx, caseName + "Wrong set of 'hasValue'!");

            if (ap[definedFlag].value.isRequired)
                return TAP_FAIL(ctx, caseName + "Wrong 'isRequired'!");

            if (ap[definedFlag].value.str != definedValue)
                return TAP_FAIL(ctx, caseName + "Different values!");
        }
    }

    return TAP_PASS(ctx, "Parse non typical flags.");
}

TestContext::Return testParserFunc(TestContext* ctx)
{
    TAP_VALUE_STR_TEST_CASES(defaultValueCases);
    TAP_REQUIRED_TEST_CASES(requiredCases);
    TAP_FLAGS_NAME_TEST_CASES(flagStrCases, "--long", "-s");

    for (size_t defaultValueCase = 0; defaultValueCase < TAP_ARRAY_SIZE(defaultValueCases); ++defaultValueCase)
    for (size_t requiredCase = 0; requiredCase < TAP_ARRAY_SIZE(requiredCases); ++requiredCase)
    for (size_t flagStrCase = 0; flagStrCase < TAP_ARRAY_SIZE(flagStrCases); ++flagStrCase) {
        const std::string defaultValueStr = defaultValueCases[defaultValueCase].str;
        const bool required = requiredCases[requiredCase].required;

        TAP_VALUE_TEST_CASES(valueCases, defaultValueStr, required, "value", "Test value.");

        for (size_t valueCase = 0; valueCase < TAP_ARRAY_SIZE(valueCases); ++valueCase) {
            const std::string longFlagStr = flagStrCases[flagStrCase].defFlagLong;
            const std::string shortFlagStr = flagStrCases[flagStrCase].defFlagShort;
            const Value value = valueCases[valueCase].value;

            struct {
                 const Flag flag;
            } testCases[] = {
                { Flag(longFlagStr, shortFlagStr, "") },
                { Flag(longFlagStr, shortFlagStr, "", value) },
            };

            for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase) {
                const Flag testFlag = testCases[testCase].flag;
                const std::string caseName = TAP_CASE_NAME(testCase, TAP_FLAG_TO_STR(testFlag));

                const std::string recievedFlag = testFlag._longFlag.size() ? testFlag._longFlag : testFlag._shortFlag;
                const std::string recievedValue = testFlag.value.str;
                char* argv[] = { TAP_CHARS("program"), TAP_CHARS(recievedFlag.c_str()), TAP_CHARS(recievedValue.c_str()) };
                const int argc = TAP_ARRAY_SIZE(argv);

                ArgParse ap;

                const Flag& resultFlag = ap.def(Flag(testFlag));

                const bool parseRet = ap.parse(argc, argv);

                if (testFlag.isValid()) {
                    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));
                    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, ap, 0);

                    if (TAP_CHECK(ctx, &resultFlag == &Flag::WrongFlag))
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (TAP_CHECK(ctx, !ap[recievedFlag].isSet))
                        return TAP_FAIL(ctx, caseName + "The flag is not set!");
                }  else {
                    if (TAP_CHECK(ctx, &resultFlag != &Flag::WrongFlag))
                        return TAP_FAIL(ctx, caseName + "!!!");
                }
            }
        }
    }

    return TAP_PASS(ctx, "The ArgParse::parse(ARGC, ARGV) test.");
}

} // namespace anonymous

void unitParserTests(TestContext* ctx)
{
    ctx->add(testSomeTypicalFlags);
    ctx->add(testUndefinedFlags);
    ctx->add(testNonTypicalFlags);
    ctx->add(testParserFunc);
}

} // namespace testargparse
