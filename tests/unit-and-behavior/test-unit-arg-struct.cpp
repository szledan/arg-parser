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
    TAP_VALUE_STR_TEST_CASES(nameCases);
    TAP_REQUIRED_TEST_CASES(requiredCases);
    TAP_VALUE_STR_TEST_CASES(valueStrCases);

    for (size_t nameCase = 0; nameCase < TAP_ARRAY_SIZE(nameCases); ++nameCase)
    for (size_t requiredCase = 0; requiredCase < TAP_ARRAY_SIZE(requiredCases); ++requiredCase)
    for (size_t valueStrCase = 0; valueStrCase < TAP_ARRAY_SIZE(valueStrCases); ++valueStrCase) {
        const bool required = requiredCases[requiredCase].required;
        const std:: string testStr = valueStrCases[valueStrCase].str;
        const std:: string testValueName = "value-name";
        const std:: string testValueDescription = "value-description";

        TAP_VALUE_TEST_CASES(valueCases, testStr, required, testValueName, testValueDescription);

        for (size_t valueCase = 0; valueCase < TAP_ARRAY_SIZE(valueCases); ++valueCase) {
            const Value value = valueCases[valueCase].value;
            const std::string testName = nameCases[nameCase].str;
            const std::string testDescription = "description";

            struct {
                Arg defined;
                struct {
                    bool isRequired;
                    bool isSet;
                    std::string str;
                    std::vector<std::string> _chooseList;
                    std::string _name;
                    std::string _description;
                    bool isDefined;
                } expected;
            }
            test = { { Arg() }, { false, false, "", {}, "", "", false } },
            testCases[] = {
                { { Arg() }, { false, false, "", {}, "", "", false } },
                { { Arg(testName) }, { false, false, "", {}, testName, "", !testName.empty() } },
                { { Arg(testName, testDescription) }, { false, false, "", {}, testName, testDescription, !testName.empty() } },
                { { Arg(testName, testDescription, required) }, { required, false, "", {}, testName, testDescription, !testName.empty() } },
                { { Arg(testName, testDescription, required, value) }, { required, false, value.str, value._chooseList, testName, testDescription, !testName.empty() } },
                { { Arg(value) }, { false, false, value.str, value._chooseList, "", "", false } },
            };

            for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase) {
                assert((sizeof(testCases[0].defined) == sizeof(testCases[0].expected)) && "Structs size are differents.");
                test.defined = testCases[testCase].defined;
                test.expected = testCases[testCase].expected;
                const std::string caseName = std::to_string(testCase) + ".(" + std::to_string(required) + ",value case:" + std::to_string(valueCase) + ") testcase. ";

                if (TAP_CHECK(ctx, test.defined.isRequired != test.expected.isRequired))
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (TAP_CHECK(ctx, test.defined.isSet != test.expected.isSet))
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (TAP_CHECK(ctx, test.defined.str != test.expected.str))
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (TAP_CHECK(ctx, test.defined._chooseList.size() != test.expected._chooseList.size())) {
                    return TAP_FAIL(ctx, caseName + "!!!");
                } else if (test.defined._chooseList.size()) {
                    for (size_t i = 0; i < test.defined._chooseList.size(); ++i)
                        if (TAP_CHECK(ctx, test.defined._chooseList[i] != test.expected._chooseList[i]))
                            return TAP_FAIL(ctx, caseName + "!!!");
                }

                if (TAP_CHECK(ctx, test.defined._name != test.expected._name))
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (TAP_CHECK(ctx, test.defined._description != test.expected._description))
                    return TAP_FAIL(ctx, caseName + "!!!");

                if (TAP_CHECK(ctx, test.defined.isDefined != test.expected.isDefined))
                    return TAP_FAIL(ctx, caseName + "!!!");
            }
        }
    }

    return TAP_PASS(ctx, "Empty constructor test.");
}

} // namespace anonymous

void unitArgStructTests(TestContext* ctx)
{
    ctx->add(testConstructors);
}

} // namespace testargparse
