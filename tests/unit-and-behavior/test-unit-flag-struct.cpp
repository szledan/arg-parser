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

TestContext::Return testConstructors(TestContext* ctx)
{
    const std::string longFlag = "--long";
    const std::string shortFlag = "-s";

    TAP_FLAGS_NAME_TEST_CASES(flagNameCases, longFlag, shortFlag);
    TAP_REQUIRED_TEST_CASES(requiredCases);
    TAP_VALUE_STR_TEST_CASES(valueStrCases);

    for (size_t flagCase = 0; flagCase < TAP_ARRAY_SIZE(flagNameCases); ++flagCase)
    for (size_t requiredCase = 0; requiredCase < TAP_ARRAY_SIZE(requiredCases); ++requiredCase)
    for (size_t valueStrCase = 0; valueStrCase < TAP_ARRAY_SIZE(valueStrCases); ++valueStrCase) {
        const bool required = requiredCases[requiredCase].required;
        const std:: string testStr = valueStrCases[valueStrCase].str;
        const std:: string testValueName = "value-name";
        const std:: string testValueDescription = "value-description";

        TAP_VALUE_TEST_CASES(valueCases, testStr, required, testValueName, testValueDescription);

        for (size_t valueCase = 0; valueCase < TAP_ARRAY_SIZE(valueCases); ++valueCase) {
            const Value value = valueCases[valueCase].value;
            const std::string longFlagName = flagNameCases[flagCase].defFlagLong;
            const std::string shortFlagName = flagNameCases[flagCase].defFlagShort;
            const std::string testDescription = "description";

            struct {
                Flag defined;
                struct {
                    bool isSet;
                    bool isDefined;
                    bool hasValue;
                    Value value;
                    std::string _longFlag;
                    std::string _shortFlag;
                    std::string _description;
                    CallBackFunc _callBackFunc;
                } expected;
            }
            test = { { Flag() }, { false, false, false, Value(), "", "", "", nullptr } },
            testCases[] = {
                { { Flag() }, { false, false, false, Value(), "", "", "", nullptr } },
                { { Flag(longFlagName, shortFlagName) }, { false, !longFlagName.empty() || !shortFlagName.empty(), false, Value(), longFlagName, shortFlagName, "", nullptr } },
                { { Flag(longFlagName, shortFlagName, testDescription) }, { false, !longFlagName.empty() || !shortFlagName.empty(), false, Value(), longFlagName, shortFlagName, testDescription, nullptr } },
                { { Flag(longFlagName, shortFlagName, testDescription, value) }, { false, !longFlagName.empty() || !shortFlagName.empty(), true, value, longFlagName, shortFlagName, testDescription, nullptr } },
            };

            for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase) {
                assert((sizeof(testCases[0].defined) == sizeof(testCases[0].expected)) && "Structs size are differents.");
                test.defined = testCases[testCase].defined;
                test.expected = testCases[testCase].expected;
                const std::string caseName = std::to_string(testCase)
                        + ".(" + std::to_string(required)
                        + ",value case:" + std::to_string(valueCase)
                        + ",flag case:" + std::to_string(flagCase) + ") testcase. ";

                if (test.defined.isSet != test.expected.isSet)
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (test.defined.isDefined != test.expected.isDefined)
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (test.defined.hasValue != test.expected.hasValue)
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (test.defined.hasValue) {
                    if (test.defined.value.isRequired != test.expected.value.isRequired)
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (test.defined.value.isSet != test.expected.value.isSet)
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (test.defined.value.str != test.expected.value.str)
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (test.defined.value._chooseList.size() != test.expected.value._chooseList.size())
                        return TAP_FAIL(ctx, caseName + "!!!");
                    else if (test.defined.value._chooseList.size()) {
                        for (size_t i = 0; i < test.defined.value._chooseList.size(); ++i)
                            if (test.defined.value._chooseList[i] != test.expected.value._chooseList[i])
                                return TAP_FAIL(ctx, caseName + "!!!");
                    }

                    if (test.defined.value._name != test.expected.value._name)
                        return TAP_FAIL(ctx, caseName + "!!!");

                    if (test.defined.value._description != test.expected.value._description)
                        return TAP_FAIL(ctx, caseName + "!!!");
                }

                if (test.defined._longFlag != test.expected._longFlag)
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (test.defined._shortFlag != test.expected._shortFlag)
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (test.defined._description != test.expected._description)
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (test.defined._callBackFunc != test.expected._callBackFunc)
                    return TAP_FAIL(ctx, caseName + "!!!");
            }
        }
    }

    return TAP_PASS(ctx, "Empty constructor test.");
}

} // namespace anonymous

void unitFlagStructTests(TestContext* ctx)
{
    ctx->add(testConstructors);
}

} // namespace testargparse

#if 0
#include "test.h"

#include "arg-parse.h"

#include <assert.h>

namespace testargparse {
namespace {

using namespace argparse;

const std::string g_shortFlag = "-a";
const std::string g_longFlag = "--a";
const std::string g_description = "Simple settable flag without value.";

TestContext::Return testShortFlagNotSet(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.def(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    return TAP_PASS(ctx, "Simple short flag test where flag doesn't set.");
}

TestContext::Return testShortFlagOnlySet(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program"), TAP_CHARS("-a") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.def(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (!args[g_shortFlag].isSet)
        return TAP_FAIL(ctx, "The flag is not set!");

    if (args[g_shortFlag].hasValue)
        return TAP_FAIL(ctx, "Wrong 'hasValue' set to true!");

    if (&(args[g_shortFlag]) != &(args[g_longFlag]))
        return TAP_FAIL(ctx, "The short and long flag return values not same!");

    if (!args[g_longFlag].isSet)
        return TAP_FAIL(ctx, "The flag is not set!");

    if (args[g_longFlag].hasValue)
        return TAP_FAIL(ctx, "Wrong 'hasValue' set to true!");

    return TAP_PASS(ctx, "Simple short flag test.");
}

TestContext::Return testLongFlagNotSet(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.def(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    return TAP_PASS(ctx, "Simple long flag test where flag doesn't set.");
}

TestContext::Return testLongFlagOnlySet(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program"), TAP_CHARS("--a") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.def(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (!args[g_longFlag].isSet)
        return TAP_FAIL(ctx, "The flag is not set!");

    if (args[g_longFlag].hasValue)
        return TAP_FAIL(ctx, "Wrong 'hasValue' set to true!");

    if (&(args[g_shortFlag]) != &(args[g_longFlag]))
        return TAP_FAIL(ctx, "The short and long flag return values not same!");

    if (!args[g_shortFlag].isSet)
        return TAP_FAIL(ctx, "The flag is not set!");

    if (args[g_shortFlag].hasValue)
        return TAP_FAIL(ctx, "Wrong 'hasValue' set to true!");

    return TAP_PASS(ctx, "Simple long flag test.");
}

TestContext::Return testLongAndShortFlagWithValueNotNeeded(TestContext* ctx)
{
    std::string value("--v");

    ArgParse args;

    args.def(Flag("", g_shortFlag, g_description, Value("w", "value")));
    args.def(Flag(g_longFlag, "", g_description, Value("w", "value")));

    struct {
        const std::string givenFlag;
        const std::string definedFlag;
    } testCases[] = {
        { "--a", g_longFlag },
        { std::string("--a=") + value, g_longFlag },
        { "-a", g_shortFlag },
    };

    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase)
    {
        const std::string givenFlag = testCases[testCase].givenFlag;
        const std::string definedFlag = testCases[testCase].definedFlag;
        const std::string caseName = givenFlag + "|" + definedFlag + " testcase. ";

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(givenFlag.c_str()), TAP_CHARS(value.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        const bool parseRet = args.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

        if (!args[definedFlag].isSet)
            return TAP_FAIL(ctx, caseName + "The flag is not set!");

        if (!args[definedFlag].hasValue)
            return TAP_FAIL(ctx, caseName + "Wrong set of 'hasValue'!");

        if (args[definedFlag].value.isRequired)
            return TAP_FAIL(ctx, caseName + "Wrong '_isValueNeeded'!");

        if (args[definedFlag].value.str != value)
            return TAP_FAIL(ctx, caseName + "Different values!");
    }

    return TAP_PASS(ctx, "Long and short flag tests with not needed value.");
}

TestContext::Return testLongAndShortFlagWithNeededValue(TestContext* ctx)
{
    std::string value("-v");

    ArgParse args;

    args.def(Flag("", g_shortFlag, g_description, Value("", "value")));
    args.def(Flag(g_longFlag, "", g_description, Value("", "value")));

    struct {
        const std::string givenFlag;
        const std::string definedFlag;
        const std::string definedValue;
    } testCases[] = {
        { "--a", g_longFlag, value },
        { std::string("--a=") + value, g_longFlag, "" },
        { "-a", g_shortFlag, value },
    };

    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase)
    {
        const std::string givenFlag = testCases[testCase].givenFlag;
        const std::string definedFlag = testCases[testCase].definedFlag;
        const std::string definedValue = testCases[testCase].definedValue;
        const std::string caseName = givenFlag + "|" + definedFlag + " testcase. ";

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(givenFlag.c_str()), TAP_CHARS(definedValue.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        const bool parseRet = args.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

        if (!args[definedFlag].isSet)
            return TAP_FAIL(ctx, caseName + "The flag is not set!");

        if (!args[definedFlag].hasValue)
            return TAP_FAIL(ctx, caseName + "Wrong set of 'hasValue'!");

        if (!args[definedFlag].value.isRequired)
            return TAP_FAIL(ctx, caseName + "Wrong '_isValueNeeded'!");

        if (args[definedFlag].value.str != value)
            return TAP_FAIL(ctx, caseName + "Different values!");
    }

    return TAP_PASS(ctx, "Long and short flag tests with not needed value.");
}

} // namespace anonymous

void unitFlagStructTests(TestContext* ctx)
{
    ctx->add(testShortFlagNotSet);
    ctx->add(testShortFlagOnlySet);
    ctx->add(testLongFlagNotSet);
    ctx->add(testLongFlagOnlySet);
    ctx->add(testLongAndShortFlagWithValueNotNeeded);
    ctx->add(testLongAndShortFlagWithNeededValue);
}

} // namespace testargparse
#endif
