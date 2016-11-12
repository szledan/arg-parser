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

namespace testargparse {
namespace {

using namespace argparse;

TestContext::Return testDefFlag(TestContext* ctx)
{
    const std::string longFlag = "--long";
    const std::string shortFlag = "--l";
    TAP_FLAGS_NAME_TEST_CASES(flagNamesCases, longFlag, shortFlag);
    struct {
        const std::string flag;
    } givens[] = {
        { longFlag },
        { shortFlag },
    };

    for (size_t given = 0; given < TAP_ARRAY_SIZE(givens); ++given)
    for (size_t nameCase = 0; nameCase < TAP_ARRAY_SIZE(flagNamesCases); ++nameCase)
    {
        const std::string& givenFlag = givens[given].flag;
        const Flag defFlag(flagNamesCases[nameCase].defFlagLong, flagNamesCases[nameCase].defFlagShort, "<descript>");
        const std::string caseName = givenFlag + "|"
                + "Flag(" + defFlag._longFlag + "," + defFlag._shortFlag + "," + defFlag._description + ")"
                + " testcase. ";

        ArgParse args;
        /*const Flag& addedFlag = */args.def(defFlag);

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(givenFlag.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        const bool parseRet = args.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

        if (&args[defFlag._longFlag] == &Flag::WrongFlag)
            return TAP_FAIL(ctx, caseName + "Args is empty.");

        if (&args[defFlag._shortFlag] == &Flag::WrongFlag)
            return TAP_FAIL(ctx, caseName + "Args is empty.");

        if (!(args.counts.flags.defined + args.counts.flags.undefined))
            return TAP_FAIL(ctx, caseName + "Args is empty.");
    }

    return TAP_PASS(ctx, "Define arguments.");
}

TestContext::Return testDefFlagWithValue(TestContext* ctx)
{
    return TAP_NOT_TESTED(ctx, "No implemented test case!!!");
}

TestContext::Return testDefFlagWithCallBackFunction(TestContext* ctx)
{
    return TAP_NOT_TESTED(ctx, "No implemented test case!!!");
}

TestContext::Return testDefArg(TestContext* ctx)
{
    TAP_ARG_NAME_TEST_CASES(nameCases);
    TAP_ARG_REQUIRED_TEST_CASES(requiredCases);

    for (size_t requiredCase = 0; requiredCase < TAP_ARRAY_SIZE(requiredCases); ++requiredCase)
    for (size_t nameCase = 0; nameCase < TAP_ARRAY_SIZE(nameCases); ++nameCase)
    {
        const std::string givenValue = "value";
        const Arg defArg(nameCases[nameCase].defArgName, "<descript>", requiredCases[requiredCase].defArgRequired);
        const std::string caseName = givenValue + "|"
                + "Arg(" + defArg._name + "," + defArg._description + "," + std::to_string(defArg._isArgNeeded) + ")"
                + " testcase. ";

        ArgParse args;
        const Arg& addedArg = args.def(defArg);

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(givenValue.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        const bool parseRet = args.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

        if (&args[0] == &Arg::WrongArg)
            return TAP_FAIL(ctx, caseName + "Args is empty.");

        if (!(args.counts.args.defined + args.counts.args.undefined))
            return TAP_FAIL(ctx, caseName + "Args is empty.");

        if (defArg._name.empty()) {
            if (&addedArg != &Arg::WrongArg)
                return TAP_FAIL(ctx, caseName + "Arg with empty name doesn't equal with WrongArg.");

            if (args[0].defined || !args.counts.args.undefined || args.counts.args.defined)
                return TAP_FAIL(ctx, caseName + "The 'given value' added as defined instead of undefined.");
        } else {
            if (&addedArg == &Arg::WrongArg)
                return TAP_FAIL(ctx, caseName + "Arg with name is equal with WrongArg.");

            if (!args[0].defined || args.counts.args.undefined || !args.counts.args.defined)
                return TAP_FAIL(ctx, caseName + "The 'given value' added as undefined instead of defined.");
        }
    }

    return TAP_PASS(ctx, "Define arguments.");
}

TestContext::Return testDefArgWithValue(TestContext* ctx)
{
    const std::string givenValue = "value";

    TAP_ARG_NAME_TEST_CASES(nameCases);
    TAP_ARG_REQUIRED_TEST_CASES(requiredCases);
    TAP_VALUE_TEST_CASES(valueCases, givenValue);

    for (size_t valueCase = 0; valueCase < TAP_ARRAY_SIZE(valueCases); ++valueCase)
    for (size_t requiredCase = 0; requiredCase < TAP_ARRAY_SIZE(requiredCases); ++requiredCase)
    for (size_t nameCase = 0; nameCase < TAP_ARRAY_SIZE(nameCases); ++nameCase)
    {
        const Arg defArg(nameCases[nameCase].defArgName, "description", requiredCases[requiredCase].defArgRequired, valueCases[valueCase].defValue);
        const std::string caseName = givenValue + "|"
                + "Arg(" + defArg._name + "," + defArg._description + "," + std::to_string(defArg._isArgNeeded)
                    + ",Value('" + defArg.str + "')"
                + ")" + " testcase. ";

        ArgParse args;
        const Arg& addedArg = args.def(defArg);

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(givenValue.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        const bool parseRet = args.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

        if (&args[0] == &Arg::WrongArg)
            return TAP_FAIL(ctx, caseName + "Args is empty.");

        if (!(args.counts.args.defined + args.counts.args.undefined))
            return TAP_FAIL(ctx, caseName + "Args is empty.");

        if (defArg._name.empty()) {
            if (&addedArg != &Arg::WrongArg)
                return TAP_FAIL(ctx, caseName + "Arg with empty name doesn't equal with WrongArg.");

            if (args[0].defined || !args.counts.args.undefined || args.counts.args.defined)
                return TAP_FAIL(ctx, caseName + "The 'parameter' added as defined instead of undefined.");
        } else {
            if (&addedArg == &Arg::WrongArg)
                return TAP_FAIL(ctx, caseName + "Arg with name is equal with WrongArg.");

            if (!args[0].defined || args.counts.args.undefined || !args.counts.args.defined)
                return TAP_FAIL(ctx, caseName + "The 'parameter' added as undefined instead of defined.");

            if (addedArg.str != givenValue)
                return TAP_FAIL(ctx, caseName + "The value string of defined arg didn't set to given value.");
        }
    }

    return TAP_PASS(ctx, "Define arguments with vlaues.");
}

} // namespace anonymous

void unitDefTests(TestContext* ctx)
{
    ctx->add(testDefFlag);
    ctx->add(testDefFlagWithValue);
    ctx->add(testDefFlagWithCallBackFunction);
    ctx->add(testDefArg);
    ctx->add(testDefArgWithValue);
}

} // namespace testargparse
