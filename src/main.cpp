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
#include <iostream>

using namespace argparse;

void allFlagFunc(void)
{
    std::cout << "The '--all' or '-a' was set." << std::endl;
}

int main(int argc, char* argv[])
{
    ArgParse args(argc, argv);
    args.add(Arg("func", "Select function.", Arg::IsNeeded));
    args.add(Arg("method", "Select method."));
    args.add(Flag("--all", "-a", "Select all functions."), &allFlagFunc);
    args.add(Flag("--list", "-l", "List all functions."));
    args.add(Flag("--choose", "-c", "Choose from [A|B|C].", Value("A", {"A", "B", "C"})));
    args.add(Flag("--file", "-f", "Set filename.", Value("arg-pars.png", "filename", "Source PNG filename.")));
    args.add(Flag("--pi", "-p", "Set pi number.", Value("3.1415")));
    args.add(Flag("--run", "", "Run program."));

    if (!args.parse())
        std::cout  << args.showHelp() << std::endl;

    std::cout  << args.showHelp() << std::endl;

//    std::string fn;
//    args.checkAndReadFleg("--file", &fn);
//    std::cout << fn << std::endl;

//    double v = 3.1416;
//    args.checkAndReadFleg("--pi", &v);
//    std::cout << v << std::endl;

    return 0;
}
