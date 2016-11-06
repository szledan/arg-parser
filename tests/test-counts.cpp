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

#include <assert.h>

namespace testargparse {
namespace {

using namespace argparse;

const std::string g_shortFlag = "-a";
const std::string g_longFlag = "--a";
const std::string g_description = "Simple settable flag without value.";

bool testNoArgNoFlag(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));
    args.add(Arg("arg", "An arg", !Arg::IsNeeded));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (args.counts.definedArgs)
        return TAP_FAIL(ctx, "Defined argument counts not null.");

    if (args.counts.undefinedArgs)
        return TAP_FAIL(ctx, "Undefined argument counts not null.");

    if (args.counts.definedFlags)
        return TAP_FAIL(ctx, "Defined flag counts not null.");

    if (args.counts.undefinedFlags)
        return TAP_FAIL(ctx, "Undefined flag counts not null.");

    return TAP_PASS(ctx, "Test counters when no args and flags.");
}

bool testDefinedArgs(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program"), TAP_CHARS("arg_1"), TAP_CHARS("arg_2"), TAP_CHARS("arg_3") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));
    args.add(Arg("arg1", "An arg 1", Arg::IsNeeded));
    args.add(Arg("arg2", "An arg 2", Arg::IsNeeded));
    args.add(Arg("arg3", "An arg 3", !Arg::IsNeeded));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (args.counts.definedArgs != 3)
        return TAP_FAIL(ctx, "Defined argument counts wrong.");

    if (args.counts.undefinedArgs)
        return TAP_FAIL(ctx, "Undefined argument counts not null.");

    if (args.counts.definedFlags)
        return TAP_FAIL(ctx, "Defined flag counts not null.");

    if (args.counts.undefinedFlags)
        return TAP_FAIL(ctx, "Undefined flag counts not null.");

    return TAP_PASS(ctx, "Test counters when only defined arguments.");
}

bool testUndefinedArgs(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program"), TAP_CHARS("arg_1"), TAP_CHARS("arg_2"), TAP_CHARS("arg_3") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (args.counts.definedArgs)
        return TAP_FAIL(ctx, "Defined argument counts not null.");

    if (args.counts.undefinedArgs != 3)
        return TAP_FAIL(ctx, "Undefined argument counts wrong.");

    if (args.counts.definedFlags)
        return TAP_FAIL(ctx, "Defined flag counts not null.");

    if (args.counts.undefinedFlags)
        return TAP_FAIL(ctx, "Undefined flag counts not null.");

    return TAP_PASS(ctx, "Test counters when only undefined arguments.");
}

bool testDefinedFlags(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program"), TAP_CHARS(g_longFlag.c_str()), TAP_CHARS(g_shortFlag.c_str()), TAP_CHARS("--other"), TAP_CHARS("value") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, "", g_description));
    args.add(Flag("", g_shortFlag, g_description));
    args.add(Flag("--other", "-o", "Other description.", Value("")));
    args.add(Arg("arg", "An arg", !Arg::IsNeeded));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (args.counts.definedArgs)
        return TAP_FAIL(ctx, "Defined argument counts not null.");

    if (args.counts.undefinedArgs)
        return TAP_FAIL(ctx, "Undefined argument counts not null.");

    if (args.counts.definedFlags != 3)
        return TAP_FAIL(ctx, "Defined flag counts wrong.");

    if (args.counts.undefinedFlags)
        return TAP_FAIL(ctx, "Undefined flag counts not null.");

    return TAP_PASS(ctx, "Test counters when only defined arguments.");
}

bool testUndefinedFlags(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program"), TAP_CHARS(g_longFlag.c_str()), TAP_CHARS(g_shortFlag.c_str()), TAP_CHARS("--other") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Arg("arg", "An arg", !Arg::IsNeeded));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (args.counts.definedArgs)
        return TAP_FAIL(ctx, "Defined argument counts not null.");

    if (args.counts.undefinedArgs)
        return TAP_FAIL(ctx, "Undefined argument counts not null.");

    if (args.counts.definedFlags)
        return TAP_FAIL(ctx, "Defined flag counts not null.");

    if (args.counts.undefinedFlags != 3)
        return TAP_FAIL(ctx, "Undefined flag counts wrong.");

    return TAP_PASS(ctx, "Test counters when only defined arguments.");
}

bool testComplex(TestContext* ctx)
{
    char* argv[] = {
        TAP_CHARS("program"),
        TAP_CHARS("arg_1"),
        TAP_CHARS(g_longFlag.c_str()),
        TAP_CHARS("arg_2"),
        TAP_CHARS(g_shortFlag.c_str()),
        TAP_CHARS("value"),
        TAP_CHARS("arg_3"),
        TAP_CHARS("--other=value"),
        TAP_CHARS("arg_4"),
    };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, "", g_description));
    args.add(Flag("", g_shortFlag, g_description, Value("")));
    args.add(Flag("--no", "-n", "Do not call this flag."));
    args.add(Arg("arg1", "An arg 1", Arg::IsNeeded));
    args.add(Arg("arg2", "An arg 2", Arg::IsNeeded));
    args.add(Arg("arg3", "An arg 3", !Arg::IsNeeded));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (args.counts.definedArgs != 3)
        return TAP_FAIL(ctx, "Defined argument counts wrong.");

    if (args.counts.undefinedArgs != 1)
        return TAP_FAIL(ctx, "Undefined argument counts wrong.");

    if (args.counts.definedFlags != 2)
        return TAP_FAIL(ctx, "Defined flag counts wrong.");

    if (args.counts.undefinedFlags != 1)
        return TAP_FAIL(ctx, "Undefined flag counts wrong.");

    return TAP_PASS(ctx, "Test counters when only defined arguments.");
}

} // namespace anonymous

void countsTests(TestContext* ctx)
{
    ctx->add(testNoArgNoFlag);
    ctx->add(testDefinedArgs);
    ctx->add(testUndefinedArgs);
    ctx->add(testDefinedFlags);
    ctx->add(testUndefinedFlags);
    ctx->add(testComplex);
}

} // namespace testargparse
