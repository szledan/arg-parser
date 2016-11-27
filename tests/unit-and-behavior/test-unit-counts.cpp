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
#include <vector>

namespace testargparse {
namespace {

using namespace argparse;

#define TAP_TEST_COUNTS_DEF(CTX, COUNTS_DEF, EXPECTED_DEF) do { \
        if (TAP_CHECK(CTX, COUNTS_DEF.flags != EXPECTED_DEF.flags)) \
            return TAP_FAIL(CTX, "!!!"); \
        if (TAP_CHECK(CTX, COUNTS_DEF.args.required != EXPECTED_DEF.args.required)) \
            return TAP_FAIL(CTX, "!!!"); \
        if (TAP_CHECK(CTX, COUNTS_DEF.args.nonRequired != EXPECTED_DEF.args.nonRequired)) \
            return TAP_FAIL(CTX, "!!!"); \
        if (TAP_CHECK(CTX, COUNTS_DEF.args.all() != EXPECTED_DEF.args.required + EXPECTED_DEF.args.nonRequired)) \
            return TAP_FAIL(CTX, "!!!"); \
        if (TAP_CHECK(CTX, COUNTS_DEF.all() != EXPECTED_DEF.flags + EXPECTED_DEF.args.all())) \
            return TAP_FAIL(CTX, "!!!"); \
    } while (false)

#define TAP_TEST_COUNTS(CTX, AP, EXPECTED) do { \
        TAP_TEST_COUNTS_DEF(CTX, AP.counts.defined, EXPECTED.defined); \
        TAP_TEST_COUNTS_DEF(CTX, AP.counts.parsed.defined, EXPECTED.parsed.defined); \
        TAP_TEST_COUNTS_DEF(CTX, AP.counts.parsed.undefined, EXPECTED.parsed.undefined); \
    } while (false)

TestContext::Return testNoArgNoFlag(TestContext* ctx)
{
    ArgParse::Counts expected;

    char* argv[] = { nullptr };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse ap("help.add=0");

    ap.parse(argc, argv);

    TAP_TEST_COUNTS(ctx, ap, expected);

    return TAP_PASS(ctx, "Test counters when no args and no flags.");
}

TestContext::Return testDefined(TestContext* ctx)
{
    ArgParse::Counts expected;

    expected.defined.flags++; // Initial adding of '--help'.
    ArgParse ap;

    ap.def(Flag());
    expected.defined.flags++; ap.def(Flag("--long", "-l", "Simple Flag"));
    ap.def(Flag("--long", "-l", "Double defination of a Flag"));
    expected.defined.flags++; ap.def(Flag("--LONG", "-L", "Flag with value", Value("value")));

    ap.def(Arg());
    expected.defined.args.required++; ap.def(Arg("arg", "Simple Arg"));
    expected.defined.args.required++; ap.def(Arg("arg", "Required Arg with value", Arg::Required, Value("value")));
    expected.defined.args.required++; ap.def(Arg("arg", "Required Arg without value", Arg::Required));
    expected.defined.args.nonRequired++; ap.def(Arg("arg", "Non required Arg with value", !Arg::Required, Value("value")));
    expected.defined.args.nonRequired++; ap.def(Arg("arg", "Non required Arg without value", !Arg::Required));

    TAP_TEST_COUNTS(ctx, ap, expected);

    return TAP_PASS(ctx, "Test struct of counter.defined.");
}

TestContext::Return testParsed(TestContext* ctx)
{
    ArgParse::Counts expected;

    expected.defined.flags++; // Initial adding of '--help'.
    ArgParse ap;

    ap.def(Flag());
    expected.defined.flags++; ap.def(Flag("--long", "-l", "Simple Flag"));
    ap.def(Flag("--long", "-l", "Double defination of a Flag"));
    expected.defined.flags++; ap.def(Flag("--LONG", "-L", "Flag with value", Value("value")));

    ap.def(Arg());
    expected.defined.args.required++; ap.def(Arg("arg", "Simple Arg"));
    expected.defined.args.required++; ap.def(Arg("arg", "Required Arg with value", Arg::Required, Value("value")));
    expected.defined.args.required++; ap.def(Arg("arg", "Required Arg without value", Arg::Required));
    expected.defined.args.nonRequired++; ap.def(Arg("arg", "Non required Arg with value", !Arg::Required, Value("value")));
    expected.defined.args.nonRequired++; ap.def(Arg("arg", "Non required Arg without value", !Arg::Required));

    std::vector<char*> argv;
    expected.parsed.defined.args.required++; argv.push_back(TAP_CHARS("arg"));
    expected.parsed.defined.args.required++; argv.push_back(TAP_CHARS("arg"));
    expected.parsed.defined.args.required++; argv.push_back(TAP_CHARS("arg"));
    expected.parsed.defined.args.nonRequired++; argv.push_back(TAP_CHARS("arg"));
    expected.parsed.defined.args.nonRequired++; argv.push_back(TAP_CHARS("arg"));
    expected.parsed.undefined.args.nonRequired++; argv.push_back(TAP_CHARS("arg"));
    expected.parsed.defined.flags++; argv.push_back(TAP_CHARS("--long"));
    expected.parsed.undefined.flags++; argv.push_back(TAP_CHARS("--short"));
    expected.parsed.defined.flags++; argv.push_back(TAP_CHARS("-l"));
    expected.parsed.undefined.flags++; argv.push_back(TAP_CHARS("-s"));

    ap.parse(argv.size(), argv.data());

    TAP_TEST_COUNTS(ctx, ap, expected);

    return TAP_PASS(ctx, "Test struct of counter.parsed.");
}

#undef TAP_TEST_COUNTS
#undef TAP_TEST_COUNTS_DEF

} // namespace anonymous

void unitCountsTests(TestContext* ctx)
{
    ctx->add(testNoArgNoFlag);
    ctx->add(testDefined);
    ctx->add(testParsed);
}

} // namespace testargparse
