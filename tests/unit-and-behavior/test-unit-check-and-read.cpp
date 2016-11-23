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

TestContext::Return testNoFlag(TestContext* ctx)
{
    return TAP_NOT_TESTED(ctx, "Check no flag in 'args'.");
}

TestContext::Return testNotSetFlag(TestContext* ctx)
{
    return TAP_NOT_TESTED(ctx, "Check not setted flag.");
}

TestContext::Return testSetFlagNoValue(TestContext* ctx)
{
    return TAP_NOT_TESTED(ctx, "Check setted flag without Value.");
}

TestContext::Return testSetFlagWithValueDifferentTypes(TestContext* ctx)
{
    return TAP_NOT_TESTED(ctx, "Check setted flag with different values.");
}

} // namespace anonymous

void unitCheckAndReadTests(TestContext* ctx)
{
    ctx->add(testNoFlag);
    ctx->add(testNotSetFlag);
    ctx->add(testSetFlagNoValue);
    ctx->add(testSetFlagWithValueDifferentTypes);
}

} // namespace testargparse
