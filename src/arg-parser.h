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

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

/*** Interface ***************************************************************/

/*! \brief Return number of unparsed arguments */
#define UNPARSED_COUNT() (ap::s_argv.size() - 1)

/*! \brief Check flags */
#define HAS_FLAG(FLAG, ARGC, ARGV) [&]()->bool { for (int _i = 1; _i < ARGC; ++_i) if (std::string(FLAG) == std::string(ARGV[_i])) return true; return false; }()

/*! \brief Initialize the parser and define the help flag */
#define PARSE_HELP(FLAGS, DEFAULT, MSG, USAGE, ARGC, ARGV) [&](){\
    /* copy argv */ ap::s_argv.resize(ARGC); for (int i = 0; i < ARGC; ++i) ap::s_argv[i] = std::string(ARGV[i]);\
    /* check help */ if (HAS_FLAGS(FLAGS)) { ap::s_help = true; AP_STDOUT << PTRNS(USAGE, "") << std::endl; } \
    /* parse value */ return PARSE_FLAG(FLAGS, ap::s_help, MSG);\
    }()

/*! \brief Define flag */
#define PARSE_FLAG(FLAGS, DEFAULT, MSG) [&](){\
    /* show help */ if (ap::s_help) { int size = AP_ALIGNMENT - std::string(FLAGS).size() - 2; AP_STDOUT << "  " << FLAGS; std::stringstream _message(MSG); std::string msg; bool first = true; std::stringstream _def; _def << DEFAULT; while (std::getline(_message, msg, '\n')) { PTRNS(msg, _def.str()); AP_STDOUT << std::string(first ? (size > 1 ? size : 2) : AP_ALIGNMENT, ' ') << msg.erase(0, msg.find_first_not_of(' ')) << std::endl; first = false; } return DEFAULT; }\
    /* parse value */ return PARSE_FLAG_VALUE(FLAGS, DEFAULT, 1, ap::s_argv.size());\
    }()

/*! \brief Define argument */
#define PARSE_ARG(DEFAULT) [&](){\
    /* parse next argument */ auto _arg = DEFAULT; if (ap::s_argv.size() > 1) { std::stringstream ss(ap::s_argv[1]); ss >> _arg; ap::s_argv.erase(ap::s_argv.begin() + 1); } return _arg;\
    }()

/*! \brief Add message */
#define ADD_MSG(MSG) [&](){\
     if (ap::s_help) AP_STDOUT << PTRNS(MSG, "") << std::endl;\
    }()

/*** Options *****************************************************************/

#ifndef AP_STDOUT
#define AP_STDOUT std::cout
#endif // AP_STDOUT

#ifndef AP_ALIGNMENT
#define AP_ALIGNMENT 25
#endif // AP_ALIGNMENT

/*** Implementation **********************************************************/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace ap {

static std::vector<std::string> s_argv;
static bool s_help = false;

#define CHECK_FLAGS(FLAGS, FROM, TO) [&]()->size_t {\
    /* separate flags */ std::vector<std::string> flags; std::stringstream ss(FLAGS); std::string flag; while (std::getline(ss, flag, ',')) { flag.erase(0, flag.find_first_not_of(' ')); flag.erase(flag.find_last_not_of(' ') + 1); flags.push_back(flag); } std::string& lastFlag = flags.back(); if (lastFlag.find_last_of(' ') < lastFlag.size()) lastFlag.erase(lastFlag.find_last_of(' ')).erase(lastFlag.find_last_not_of(' ') + 1);\
    /* check flag */ for (size_t _i = FROM; _i < TO; ++_i) for (size_t _fi = 0; _fi < flags.size(); ++_fi) if (ap::s_argv[_i] == flags[_fi]) return _i; \
    /* not found */ return 0; }()
#define HAS_FLAGS(FLAGS) CHECK_FLAGS(FLAGS, 1, ap::s_argv.size())
#define PARSE_FLAG_VALUE(FLAGS, DEFAULT, FROM, TO) [&](){\
    /* check flag */ size_t _j = CHECK_FLAGS(FLAGS, FROM, TO);\
    /* parse value */ auto _r = DEFAULT;  if (_j) { if (typeid(DEFAULT) == typeid(bool)) { ap::s_argv.insert(ap::s_argv.begin() + _j + 1, "1"); } if ((++_j) < ap::s_argv.size()) { std::stringstream ss(ap::s_argv[_j]); ss >> _r; ap::s_argv.erase(ap::s_argv.begin() + _j); ap::s_argv.erase(ap::s_argv.begin() + _j - 1); } }\
    /* return value */ return _r;\
    }()

#define REPLACE_PATTERN(MSG, PTRN, VALUE) [&](){ std::string _str(MSG); std::string _ptrn(PTRN); while (_str.find(_ptrn) < _str.size()) _str.replace(_str.find(_ptrn), _ptrn.length(), std::string(VALUE)); return _str; }()
#define PTRNS(STR, DEF) REPLACE_PATTERN(REPLACE_PATTERN(STR, "%d", DEF), "%p", ap::s_argv[0])

} // namespace ap

#endif // ARG_PARSER_H
