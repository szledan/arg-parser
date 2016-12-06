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

#include "arg-parser.hpp"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const ap::Arg& a)
{
    os << a.isSet << ", " << a.isRequired << ", '" << a._str << "'" << ", '" << a._default << "'";
    return os;
}

std::ostream& operator<<(std::ostream& os, const ap::Flag& f)
{
    os << f.name() << ", " << f.isSet << ", " << f.hasArg << ", Arg(" << f.arg << ")";
    return os;
}

template <typename T>
void writeFlagInfo(const ap::Flag& f)
{
    std::cout << f.name() << " = ";
    if (f.isSet)
        std::cout << f.read<T>();
    else
        std::cout << " not set";
    std::cout << "; Flag(" << f << ")." << std::endl;
}

int main(int argc, char* argv[])
{
    std::cout << "1. use case: create own param logic" << std::endl;
    struct {
        ap::Flag on = ap::Flag("--on", "-o").defHelp("Set on.");
        ap::Flag pi = ap::Flag("--pi", "-p").defArg(ap::Arg("3.15"))
                .defHelp("The pi number.");

        bool none() { return !on.isSet && !pi.isSet; }
    } params;

    {
        ap::ArgParser ap;
        ap.add(&params.on);
        ap.add(&params.pi);

        if (!ap.parse(argc, argv)) {
            std::cout << ap.help() << std::endl;
            return 1;
        }
    }

    writeFlagInfo<bool>(params.on);
    writeFlagInfo<double>(params.pi);
    std::cout << ".none(): " << params.none() << std::endl;

    std::cout << "2. use case: parse and use" << std::endl;
    {
        ap::ArgParser ap;
        ap.def(ap::Flag("--on").defHelp("Set on."));
        ap.def(ap::Flag("--pi", "-p").defArg(ap::Arg("3.15")));

        if (!ap.parse(argc, argv)) {
            std::cout << ap.help() << std::endl;
            return 1;
        }

        writeFlagInfo<bool>(ap["--on"]);
        writeFlagInfo<double>(ap["--pi"]);
        writeFlagInfo<std::string>(ap["--pu"]);
        writeFlagInfo<bool>(ap["-o"]);
        writeFlagInfo<double>(ap["-p"]);

        enum Sparse {
            AUTO = 0,
            ALWAYS,
            NEVER,
        } sparse = ap["--pi"].read<Sparse>();
    }

    return 0;
}
