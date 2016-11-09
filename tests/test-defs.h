#ifndef TEST_DEFS_H
#define TEST_DEFS_H

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

#ifndef TAP_ARRAY_SIZE
#define TAP_ARRAY_SIZE(A) (sizeof(A) ? sizeof(A) / sizeof(A[0]) : 0)
#endif // TAP_ARRAY_SIZE

#ifndef TAP_FILE_FUNC_LINE
#define TAP_FILE_FUNC_LINE std::string(__FILE__), std::string(__func__), std::to_string(__LINE__)
#endif // TAP_FILE_FUNC_LINE

#ifndef TAP_PASS
#define TAP_PASS(CTX, MSG) CTX->pass(MSG, TAP_FILE_FUNC_LINE)
#endif // TAP_PASS

#ifndef TAP_FAIL
#define TAP_FAIL(CTX, MSG) CTX->fail(MSG, TAP_FILE_FUNC_LINE)
#endif // TAP_FAIL

#ifndef TAP_CHARS
#define TAP_CHARS(STR) (char*)STR
#endif // TAP_CHARS

#ifndef TAP_CHECK_NON_REQUIRED_ERRORS
#define TAP_CHECK_NON_REQUIRED_ERRORS(CTX, ARGS, NUMS) do { \
        if (args.errors().size() != NUMS) { \
            std::string msg("Generated non required error(s)! Number Of required errors: "); \
            msg += std::to_string(NUMS) + ", but have: "; \
            msg += std::to_string(args.errors().size()) + ". "; \
            msg += "Error message(s): "; \
            for (auto const& err : args.errors()) \
                msg += err.message + " "; \
            return TAP_FAIL(ctx, msg);  \
        } \
    } while (false)

#endif // TAP_CHECK_NON_REQUIRED_ERRORS

#ifndef TAP_CHECK_PARSER_EXPECTED_RETURN
#define TAP_CHECK_PARSER_EXPECTED_RETURN(CTX, CONDITION) do { \
        if (CONDITION) { \
            return TAP_FAIL(ctx, "The parse() returns wrong value! The wrong condition is: " #CONDITION "."); \
        } \
    } while (false)

#endif // TAP_NON_REQUIRED_ERRORS

#endif // TEST_DEFS_H
