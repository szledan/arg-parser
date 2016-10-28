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

namespace unittest {
namespace {

using namespace argparse;

const std::string g_shortFlag = "-a";
const std::string g_longFlag = "--a";
const std::string g_description = "Simple settable flag without value.";

bool testShortFlagNotSet(TestContext* ctx)
{
    char* argv[] = { "program" };
    const int argc = UT_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    UT_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    UT_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    return UT_PASS(ctx, "Simple short flag tests");
}

bool testShortFlagOnlySet(TestContext* ctx)
{
    char* argv[] = { "program", "-a" };
    const int argc = UT_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    UT_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    UT_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (!args[g_shortFlag]._isSet)
        return UT_FAIL(ctx, "The flag is not set!");

    if (args[g_shortFlag]._hasValue)
        return UT_FAIL(ctx, "Wrong '_hasValue' set to true!");

    if (&(args[g_shortFlag]) != &(args[g_longFlag]))
        return UT_FAIL(ctx, "The short and long flag return values not same!");

    if (!args[g_longFlag]._isSet)
        return UT_FAIL(ctx, "The flag is not set!");

    if (args[g_longFlag]._hasValue)
        return UT_FAIL(ctx, "Wrong '_hasValue' set to true!");

    return UT_PASS(ctx, "Simple short flag tests");
}

} // namespace anonymous

void flagTests(TestContext* ctx)
{
    ctx->add(testShortFlagNotSet);
    ctx->add(testShortFlagOnlySet);
}

} // namespace unittest
