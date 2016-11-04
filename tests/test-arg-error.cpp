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

#include <assert.h>
#include <iostream> // only for testing, delete it!

namespace unittest {
namespace {

using namespace argparse;

bool testArgCountEmptyArgv(TestContext* ctx)
{
    char* argv[] = { nullptr };
    const int argc = 2;

    ArgParse args;
    const bool parseRet = args.parse(argc, argv);

    UT_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet == true));

    if (!args.errors().size())
        return UT_FAIL(ctx, "Missing error logging after 'argc' is '0'!");

    if (args.errors()[0].errorCode != ArgParse::ErrorARGVEmpty)
        return UT_FAIL(ctx, "The 'argv' is empty, but error codes is wrong!");

    if (args.errors()[0].type != ArgParse::ArgError::GeneralType)
        return UT_FAIL(ctx, "Wrong error type!");

    return UT_PASS(ctx, "ArgCount test with empty 'argv'.");
}

bool testArgCountZeroArgc(TestContext* ctx)
{
    char* argv[] = { (char*)"program" };
    const int argc = 0;

    ArgParse args;
    const bool parseRet = args.parse(argc, argv);

    UT_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet == true));

    if (!args.errors().size())
        return UT_FAIL(ctx, "Missing error logging after 'argc' is '0'!");

    if (args.errors()[0].errorCode != ArgParse::ErrorARGVEmpty)
        return UT_FAIL(ctx, "The 'argv' is empty, but error codes is wrong!");

    if (args.errors()[0].type != ArgParse::ArgError::GeneralType)
        return UT_FAIL(ctx, "Wrong error type!");

    return UT_PASS(ctx, "ArgCount test with zero 'argc'.");
}

bool testArgCountNonEmptyArgv(TestContext* ctx)
{
    char* argv[] = { (char*)"program" };
    const int argc = UT_ARRAY_SIZE(argv);

    ArgParse args;
    const bool parseRet = args.parse(argc, argv);

    UT_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    UT_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    return UT_PASS(ctx, "ArgCount test with non empty 'argv'.");
}

bool testErrorCode(TestContext* ctx)
{
    char* argv[] = { (char*)"program" };
    const int argc = UT_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Arg("arg1", "this arg needed", Arg::IsNeeded));
    args.add(Arg("arg2", "this arg needed", Arg::IsNeeded));
    args.add(Arg("arg3", "this arg not needed", !Arg::IsNeeded));

    const bool parseRet = args.parse(argc, argv);

    if (parseRet == true)
        return UT_FAIL(ctx, "The parse() return true instead of false!");

    if (!args.errors().size())
        return UT_FAIL(ctx, "Missing error logging after some args missing!");

    UT_CHECK_NON_REQUIRED_ERRORS(ctx, args, 2);

    for (auto const& e : args.errors()) {
        switch (e.errorCode) {
        case ArgParse::ErrorRequiredArgumentMissing:
            assert(e.arg && "Argument missing!");
            if (e.type != ArgParse::ArgError::ArgType)
                return UT_FAIL(ctx, "Wrong error type!");
            else if (!e.arg->IsNeeded)
                return UT_FAIL(ctx, "Wrong argument option!");
            break;
        default:
            return UT_FAIL(ctx, "Wrong ErrorCode!");
            break;
        }
    }

    return UT_PASS(ctx, "ErrorCode tests");
}

} // namespace anonymous

void argErrorTests(TestContext* ctx)
{
    ctx->add(testArgCountEmptyArgv);
    ctx->add(testArgCountZeroArgc);
    ctx->add(testArgCountNonEmptyArgv);
    ctx->add(testErrorCode);
}

} // namespace unittest
