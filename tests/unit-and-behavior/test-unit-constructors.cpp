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

namespace testargparse {
namespace {

using namespace argparse;

TestContext::Return testInterlacedString(TestContext* ctx)
{
    TAP_VALUE_STR_TEST_CASES(programNameCases);
    TAP_VALUE_STR_TEST_CASES(tabCases);
    TAP_BOOL_TEST_CASES(modeStrictCases);
    TAP_BOOL_TEST_CASES(helpAddCases);
    TAP_BOOL_TEST_CASES(helpCompactCases);
    struct {
        const int helpShow;
    } helpShowCases[] = {
        { ArgParse::Options::Help::ShowOnesWithDescription },
        { ArgParse::Options::Help::ShowAllDefined },
        { ArgParse::Options::Help::ShowAll },
    };

    for (size_t helpShowCase = 0; helpShowCase < TAP_ARRAY_SIZE(helpShowCases); ++helpShowCase)
    for (size_t helpCompactCase = 0; helpCompactCase < TAP_ARRAY_SIZE(helpCompactCases); ++helpCompactCase)
    for (size_t helpAddCase = 0; helpAddCase < TAP_ARRAY_SIZE(helpAddCases); ++helpAddCase)
    for (size_t modeStrictCase = 0; modeStrictCase < TAP_ARRAY_SIZE(modeStrictCases); ++modeStrictCase)
    for (size_t tabCase = 0; tabCase < TAP_ARRAY_SIZE(tabCases); ++tabCase)
    for (size_t programNameCase = 0; programNameCase < TAP_ARRAY_SIZE(programNameCases); ++programNameCase) {
        const std::string interlacedOptionString =
                std::string("program.name=") + programNameCases[programNameCase].str
                + "," + std::string("tab=") + tabCases[tabCase].str
                + "," + std::string("mode.strict=") + std::to_string((int)modeStrictCases[modeStrictCase].value)
                + "," + std::string("help.add=") + std::to_string((int)helpAddCases[helpAddCase].value)
                + "," + std::string("help.compact=") + std::to_string((int)helpCompactCases[helpCompactCase].value)
                + "," + std::string("help.show=") + std::to_string(helpShowCases[helpShowCase].helpShow);
        ArgParse args(interlacedOptionString);

        if (TAP_CHECK(ctx, args.options.program.name != programNameCases[programNameCase].str))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.tab != tabCases[tabCase].str))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.mode.strict != modeStrictCases[modeStrictCase].value))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.help.add != helpAddCases[helpAddCase].value))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.help.compact != helpCompactCases[helpCompactCase].value))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.help.show != helpShowCases[helpShowCase].helpShow))
            return TAP_FAIL(ctx, "!!!");
    }

    return TAP_PASS(ctx, "Construct with interlaced string.");
}

TestContext::Return testInitializerList(TestContext* ctx)
{
    TAP_VALUE_STR_TEST_CASES(programNameCases);
    TAP_VALUE_STR_TEST_CASES(tabCases);
    TAP_BOOL_TEST_CASES(modeStrictCases);
    TAP_BOOL_TEST_CASES(helpAddCases);
    TAP_BOOL_TEST_CASES(helpCompactCases);
    struct {
        const int helpShow;
    } helpShowCases[] = {
        { ArgParse::Options::Help::ShowOnesWithDescription },
        { ArgParse::Options::Help::ShowAllDefined },
        { ArgParse::Options::Help::ShowAll },
    };

    for (size_t helpShowCase = 0; helpShowCase < TAP_ARRAY_SIZE(helpShowCases); ++helpShowCase)
    for (size_t helpCompactCase = 0; helpCompactCase < TAP_ARRAY_SIZE(helpCompactCases); ++helpCompactCase)
    for (size_t helpAddCase = 0; helpAddCase < TAP_ARRAY_SIZE(helpAddCases); ++helpAddCase)
    for (size_t modeStrictCase = 0; modeStrictCase < TAP_ARRAY_SIZE(modeStrictCases); ++modeStrictCase)
    for (size_t tabCase = 0; tabCase < TAP_ARRAY_SIZE(tabCases); ++tabCase)
    for (size_t programNameCase = 0; programNameCase < TAP_ARRAY_SIZE(programNameCases); ++programNameCase) {
        ArgParse args({ std::string("program.name=") + programNameCases[programNameCase].str,
                        std::string("tab=") + tabCases[tabCase].str,
                        std::string("mode.strict=") + std::to_string((int)modeStrictCases[modeStrictCase].value),
                        std::string("help.add=") + std::to_string((int)helpAddCases[helpAddCase].value),
                        std::string("help.compact=") + std::to_string((int)helpCompactCases[helpCompactCase].value),
                        std::string("help.show=") + std::to_string(helpShowCases[helpShowCase].helpShow)
                      });

        if (TAP_CHECK(ctx, args.options.program.name != programNameCases[programNameCase].str))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.tab != tabCases[tabCase].str))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.mode.strict != modeStrictCases[modeStrictCase].value))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.help.add != helpAddCases[helpAddCase].value))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.help.compact != helpCompactCases[helpCompactCase].value))
            return TAP_FAIL(ctx, "!!!");

        if (TAP_CHECK(ctx, args.options.help.show != helpShowCases[helpShowCase].helpShow))
            return TAP_FAIL(ctx, "!!!");
    }

    return TAP_PASS(ctx, "Construct with interlaced string.");
}

} // namespace anonymous

void unitConstructorsTests(TestContext* ctx)
{
    ctx->add(testInterlacedString);
    ctx->add(testInitializerList);
}

} // namespace testargparse
