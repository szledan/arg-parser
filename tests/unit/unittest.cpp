/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Szilard Ledan <szledan@gmail.com>
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

#include "arg-parse.h"

#include <assert.h>
#include <iostream>
#include <random>

using namespace argparse;

void allFlagFunc(void)
{
    std::cout << "The '--all' or '-a' was set." << std::endl;
}

#define ARRAY_SIZE(A) (sizeof(A) ? sizeof(A) / sizeof(A[0]) : 0)
#define STR(S) const_cast<char*>(std::string(S).c_str())

static const std::string msg("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec non odio dignissim, dignissim lorem vitae, auctor dolor. Ut eu lacus ut mi finibus finibus. Donec eget condimentum elit, ut facilisis massa. Donec quis erat nulla. Maecenas a pretium nibh. Aliquam pulvinar varius mauris, id feugiat ligula sagittis non. Mauris sit amet convallis ipsum. Etiam accumsan id lorem eu tempus. In rhoncus semper maximus. Nulla quis leo vitae lacus mollis laoreet. Integer in sagittis ligula. Cras eu lacinia enim. Quisque a elit ante. Phasellus tincidunt, ipsum id tempor auctor, quam ex gravida elit, auctor porta elit dolor sit amet lorem. Ut lobortis ante lacus, quis dictum nisi condimentum nec.");

int main(int argc, char* argv[])

{
    // Manual test for showHelp().
    {
#define LINE std::string("_" + std::to_string(__LINE__))

#define ARG ("A" + LINE)
#define MSG (msg.substr(std::rand() % (msg.size() / 20), std::rand() % (msg.size() / 10)))
#define VALUEV(VL) (VL + LINE)
#define VALUE ("V" + LINE)
#define CHAR (std::string("") + (sf++))
#define SFLAG ("-" + CHAR)
#define LFLAG ("-" + SFLAG + LINE)

        char sf = 'A';
        char* argv[] = {
            STR("show-help"), // programname
        };
        int argc = ARRAY_SIZE(argv);

        ArgParse args(argc, argv);

        for (int i = 0; i < 2; ++i)
        for (int v = 0; v < 2; ++v) {
            args.add(Arg());
            args.add(Arg("", "", !(i%2)*Arg::IsNeeded, (v%2) ? Value(VALUE) : Value()));
            args.add(Arg(ARG, "", !(i%2)*Arg::IsNeeded, (v%2) ? Value(VALUE) : Value()));
            args.add(Arg(ARG, MSG, !(i%2)*Arg::IsNeeded, (v%2) ? Value(VALUE) : Value()));
        }

        args.add(Flag());
        for (int i = 0; i < 8; ++i) {
            Value value;
            switch(i) {
                case 1: value = Value(VALUE); break;
                case 2: value = Value("", ARG, MSG); break;
                case 3: value = Value(VALUE, "", MSG); break;
                case 4: value = Value(VALUE, ARG, ""); break;
                case 5: value = Value(VALUE, "", ""); break;
                case 6: value = Value(VALUEV("A"), { VALUEV("A"), VALUEV("B"), VALUEV("C") }); break;
                case 7: value = Value("", { VALUEV("A"), VALUEV("B"), VALUEV("C") }); break;
                default: value = Value(); break;
            }
            args.add(Flag("", SFLAG, "", value));
            args.add(Flag("", SFLAG, MSG, value));
            args.add(Flag(LFLAG, "", "", value));
            args.add(Flag(LFLAG, SFLAG, "", value));
            args.add(Flag(LFLAG, "", MSG, value));
            args.add(Flag(LFLAG, SFLAG, MSG, value));
        }

        std::cout  << args.showHelp() << std::endl;
    }

    // Argument tests.
    {
    }

    // Flag tests.
    {

    }

    return 0;
}
