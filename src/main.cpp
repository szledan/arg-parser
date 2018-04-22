/* Copyright (C) 2018, Szilard Ledan <szledan@gmail.com>
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

int option_main(int argc, char* argv[]);

#include "arg-parser.h"

int main(int argc, char* argv[])
{
    // 1. Simple usage in 'main'

    /* Parse help. */
    bool a_help           = PARSE_HELP("-h, --help, --usage", false, "show this help.", "Arg-parser Demo *** Simple version *** (C) 2018. Szilard Ledan\nUsage: %p [options] name number [number...]\n\nOptions:", argc, argv);
    /* Parse flags. */
    uint a_size           = PARSE_FLAG("--size SIZE", 300, "set size of window. Default is '%d'.");
    float a_lineWidth     = PARSE_FLAG("-w, --line-width LW", 3.14f, "set width of line. Default is '%d'.");
    std::string a_path    = PARSE_FLAG("-p, --path PATH", std::string("./build"), "set working dir. Default is '%d'.");
    const char a_dot      = HAS_FLAG("-d", argc, argv) ? PARSE_FLAG("-d DOT", '.', "set separate char. Default is '%d'.") : '\0';
    bool a_enable         = PARSE_FLAG("-e, --enable", false, "enable something.");
    ADD_MSG("\n Frequencies:");
    int a_frequency       = PARSE_FLAG("-f, --frequency FREQ", 60, "set rendering frequency.\n Default is '%d', but '%d' is not the best.");
    int a_Frequency       = PARSE_FLAG("+f, ++frequency FREQ", 25, "set refreshing frequency. Default is '%d'.");
    ADD_MSG("\nExamples:\n  %p -f 90 nothing 1 1 2 3 5 8\n  %p -s 2000 -e something 123 124 125");
    /* Parse arguments. */
    std::string a_from    = PARSE_ARG(std::string("ABC"));
    std::vector<int> a_to(1, PARSE_ARG(3));
    while (!a_help && UNPARSED_COUNT()) {
        a_to.push_back(PARSE_ARG(0));
    }
    /* Check help. */
    if (a_help) {
        return option_main(argc, argv); // 2. Call the Singleton solution.
    }

    std::cout << "a_help:      " << a_help << std::endl;
    std::cout << "a_frequency: " << a_frequency << std::endl;
    std::cout << "a_Frequency: " << a_Frequency << std::endl;
    std::cout << "a_size:      " << a_size << std::endl;
    std::cout << "a_lineWidth: " << a_lineWidth << std::endl;
    std::cout << "a_path:      " << a_path << std::endl;
    std::cout << "a_dot:       " << a_dot << std::endl;
    std::cout << "a_enable:    " << a_enable << std::endl;
    std::cout << "a_from:      " << a_from << std::endl;
    std::cout << "a_to:        "; for (size_t i = 0; i < a_to.size(); ++i) std::cout << a_to[i] << " "; std::cout << std::endl;

    return option_main(argc, argv); // 2. Call the Singleton solution.
}

/* 2. Singleton class variant */

#ifdef AP_ALIGNMENT
#undef AP_ALIGNMENT
#endif
#define AP_ALIGNMENT 30

#ifdef AP_STDOUT
#undef AP_STDOUT
#endif
#define AP_STDOUT Options::getOptions().m_optionStream

class Options {
public:
    static Options& getOptions()
    {
        static Options instance;
        return instance;
    }

    Options(Options const&) = delete;
    void operator=(Options const&) = delete;

    Options& parseOptions(int argc, char* argv[])
    {
        /* Parse help. */
        m_help      = PARSE_HELP("-h, --help, --usage", false, "show this help.", "Arg-parser Demo *** Singleton version *** (C) 2018. Szilard Ledan\nUsage: " + std::string(argv[0]) +  " [options] name number [number...]\n\nOptions:", argc, argv);
        /* Parse flags. */
        m_frequency = PARSE_FLAG("-f, --frequency FREQ", 60, "set rendering frequency.\n Default is '%d', but '%d' is not the best.");
        m_Frequency = PARSE_FLAG("+f, ++frequency FREQ", 25, "set refreshing frequency. Default is '%d'.");
        m_size      = PARSE_FLAG("--size SIZE", 300, "set size of window. Default is '%d'.");
        m_lineWidth = PARSE_FLAG("-w, --line-width LW", 3.14f, "set width of line. Default is '%d'.");
        m_path      = PARSE_FLAG("-p, --path PATH", std::string("./build"), "set working dir. Default is '%d'.");
        m_dot       = PARSE_FLAG("-d DOT", '.', "set separate char. Default is '%d'.");
        m_enable    = PARSE_FLAG("-e, --enable", false, "enable something.");
        /* Parse arguments. */
        m_from      = PARSE_ARG(m_from);
        m_to.push_back(PARSE_ARG(3));
        while (!m_help && UNPARSED_COUNT()) {
            m_to.push_back(PARSE_ARG(0));
        }

        return *this;
    }

    bool m_help = false;
    int m_frequency = 60;
    int m_Frequency = 25;
    uint m_size = 300;
    float m_lineWidth = 3.14;
    std::string m_path = "./build";
    char m_dot = '.';
    bool m_enable = false;
    std::string m_from = "ABC";
    std::vector<int> m_to;

    std::stringstream m_optionStream;
private:
    Options() {}

};

int option_main(int argc, char* argv[])
{
    Options& options = Options::getOptions().parseOptions(argc, argv);

    if (options.m_help) {
        std::cout << options.m_optionStream.str();
        return 0;
    }

    std::cout << "m_help:      " << options.m_help << std::endl;
    std::cout << "m_frequency: " << options.m_frequency << std::endl;
    std::cout << "m_Frequency: " << options.m_Frequency << std::endl;
    std::cout << "m_size:      " << options.m_size << std::endl;
    std::cout << "m_lineWidth: " << options.m_lineWidth << std::endl;
    std::cout << "m_path:      " << options.m_path << std::endl;
    std::cout << "m_dot:       " << options.m_dot << std::endl;
    std::cout << "m_enable:    " << options.m_enable << std::endl;
    std::cout << "m_from:      " << options.m_from << std::endl;
    std::cout << "m_to:        "; for (size_t i = 0; i < options.m_to.size(); ++i) std::cout << options.m_to[i] << " "; std::cout << std::endl;

    return 0;
}
