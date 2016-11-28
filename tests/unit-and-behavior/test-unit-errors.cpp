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
#include <iostream> // only for testing, delete it!

namespace testargparse {
namespace {

using namespace argparse;

TestContext::Return testArgArgvIsNullPtr(TestContext* ctx)
{
    ArgParse ap;
    const bool parseRet = ap.parse(1985, nullptr);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet == true));

    if (!ap.errors.data.size())
        return TAP_FAIL(ctx, "Missing error logging after 'argv' is 'nullptr'!");

    if (ap.errors.data[0].code != ArgParse::Errors::Codes::Parse_ArgVIsEmpty)
        return TAP_FAIL(ctx, "The 'argv' is 'nullptr', but error codes is wrong!");

    if (ap.errors.data[0].suspect.type != ArgParse::Errors::Error::Suspect::GeneralType)
        return TAP_FAIL(ctx, "Wrong error type!");

    return TAP_PASS(ctx, "ArgCount test with 'argv' is nullptr.");
}

TestContext::Return testArgCountEmptyArgv(TestContext* ctx)
{
    char* argv[] = { nullptr };
    const int argc = 2;

    ArgParse ap;
    const bool parseRet = ap.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet == true));

    if (!ap.errors.data.size())
        return TAP_FAIL(ctx, "Missing error logging after 'argc' is '0'!");

    if (ap.errors.data[0].code != ArgParse::Errors::Codes::Parse_ArgVIsEmpty)
        return TAP_FAIL(ctx, "The 'argv' is empty, but error codes is wrong!");

    if (ap.errors.data[0].suspect.type != ArgParse::Errors::Error::Suspect::GeneralType)
        return TAP_FAIL(ctx, "Wrong error type!");

    return TAP_PASS(ctx, "ArgCount test with empty 'argv'.");
}

TestContext::Return testArgCountZeroArgc(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = 0;

    ArgParse ap;
    const bool parseRet = ap.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet == true));

    if (!ap.errors.data.size())
        return TAP_FAIL(ctx, "Missing error logging after 'argc' is '0'!");

    if (ap.errors.data[0].code != ArgParse::Errors::Codes::Parse_ArgVIsEmpty)
        return TAP_FAIL(ctx, "The 'argv' is empty, but error codes is wrong!");

    if (ap.errors.data[0].suspect.type != ArgParse::Errors::Error::Suspect::GeneralType)
        return TAP_FAIL(ctx, "Wrong error type!");

    return TAP_PASS(ctx, "ArgCount test with zero 'argc'.");
}

TestContext::Return testArgcBiggerSizeOfArgv(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = 1985;

    ArgParse ap;
    const bool parseRet = ap.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet == true));

    if (!ap.errors.data.size())
        return TAP_FAIL(ctx, "Missing error logging after 'argc' is '0'!");

    if (ap.errors.data[0].code != ArgParse::Errors::Codes::Parse_ArgCBiggerThanElementsOfArgV)
        return TAP_FAIL(ctx, std::string("Wrong error code! The code is: ") + std::to_string(ap.errors.data[0].code));

    if (ap.errors.data[0].suspect.type != ArgParse::Errors::Error::Suspect::GeneralType)
        return TAP_FAIL(ctx, "Wrong error type!");

    return TAP_PASS(ctx, "ArgCount test with zero 'argc'.");
}

TestContext::Return testArgCountNonEmptyArgv(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse ap;
    const bool parseRet = ap.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, ap, 0);

    return TAP_PASS(ctx, "ArgCount test with non empty 'argv'.");
}

TestContext::Return testErrorCode(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse ap;

    ap.def(Arg("arg1", "this arg needed", Arg::Required));
    ap.def(Arg("arg2", "this arg needed", Arg::Required));
    ap.def(Arg("arg3", "this arg not needed", !Arg::Required));

    const bool parseRet = ap.parse(argc, argv);

    if (parseRet == true)
        return TAP_FAIL(ctx, "The parse() return true instead of false!");

    if (!ap.errors.data.size())
        return TAP_FAIL(ctx, "Missing error logging after some args missing!");

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, ap, 2);

    for (auto const& e : ap.errors.data) {
        switch (e.code) {
        case ArgParse::Errors::Codes::Parse_RequiredArgumentMissing:
            assert(e.suspect.arg && "Argument missing!");
            if (e.suspect.type != ArgParse::Errors::Error::Suspect::ArgType)
                return TAP_FAIL(ctx, "Wrong error type!");
            else if (!e.suspect.arg->Required)
                return TAP_FAIL(ctx, "Wrong argument option!");
            break;
        default:
            return TAP_FAIL(ctx, "Wrong ErrorCode!");
            break;
        }
    }

    return TAP_PASS(ctx, "ErrorCode tests");
}

} // namespace anonymous

void unitErrorsTests(TestContext* ctx)
{
    ctx->add(testArgArgvIsNullPtr);
    ctx->add(testArgCountEmptyArgv);
    ctx->add(testArgCountZeroArgc);
    ctx->add(testArgcBiggerSizeOfArgv);
    ctx->add(testArgCountNonEmptyArgv);
    ctx->add(testErrorCode);
}

} // namespace testargparse
