#ifndef TEST_UNIT_H
#define TEST_UNIT_H

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

#include "arg-parse.h"
#include "test-defs.h"
#include <string>

namespace testargparse {

void unitArgStructTests(TestContext*);
void unitCheckFlagAndReadValueTests(TestContext*);
void unitCheckFlagTests(TestContext*);
void unitConstructorsTests(TestContext*);
void unitCountsTests(TestContext*);
void unitDefTests(TestContext*);
void unitErrorsTests(TestContext*);
void unitFlagStructTests(TestContext*);
void unitOperatorTests(TestContext*);
void unitOptionsTests(TestContext*);
void unitParserTests(TestContext*);
void unitValueStructTests(TestContext*);

#ifndef TAP_FLAGS_NAME_TEST_CASES
#define TAP_FLAGS_NAME_TEST_CASES(TEST_CASES, LONG, SHORT) \
    struct { \
        const std::string defFlagLong; \
        const std::string defFlagShort; \
    } TEST_CASES[] = { \
        { "", "" }, \
        { LONG, "" }, \
        { "", SHORT }, \
        { LONG, SHORT }, \
        { "", "-" }, \
        { "--", "" }, \
        { "l", "s" }, \
    }
#endif // TAP_FLAGS_NAME_TEST_CASES

#ifndef TAP_ARG_NAME_TEST_CASES
#define TAP_ARG_NAME_TEST_CASES(TEST_CASES) \
    struct { \
        const std::string defArgName; \
    } TEST_CASES[] = { \
        { "" }, \
        { "arg" }, \
    }
#endif // TAP_ARG_NAME_TEST_CASES

#ifndef TAP_ARG_REQUIRED_TEST_CASES
#define TAP_ARG_REQUIRED_TEST_CASES(TEST_CASES) \
    struct { \
        const bool defArgRequired; \
    } TEST_CASES[] = { \
        { argparse::Arg::IsNeeded }, \
        { !argparse::Arg::IsNeeded }, \
    }
#endif // TAP_ARG_REQUIRED_TEST_CASES

#ifndef TAP_VALUE_TEST_CASES
#define TAP_VALUE_TEST_CASES(TEST_CASES, GIVEN_VALUE) \
    struct { \
        const argparse::Value defValue; \
    } TEST_CASES[] = { \
        { Value() }, \
        { Value("") }, \
        { Value(GIVEN_VALUE) }, \
        { Value("A") }, \
        { Value(GIVEN_VALUE, { GIVEN_VALUE, "other", "values" }) }, \
        { Value(GIVEN_VALUE, { "A", "B", "C" }) }, \
        { Value("", { "A", "B", "C" }) }, \
        { Value("", { }) }, \
        { Value(GIVEN_VALUE, { }) }, \
    }
#endif // TAP_VALUE_TEST_CASES

} // namespace testargparse

#endif // TEST_UNIT_H
