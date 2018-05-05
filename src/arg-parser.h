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

/*! \brief Initialize parser and define help flag */
#define PARSE_HELP(FLAGS, MSG, USAGE, ARGC, ARGV) [&](){\
    /* copy and setup argv */ for (int i = 0; i < ARGC; ++i) { std::string av = std::string(ARGV[i]); size_t pos = av.find_first_of(ap::s_long_flag_delimiter); if (std::string::npos != pos) { ap::s_argv.push_back(av.substr(0, pos)); av = av.substr(pos + 1); } ap::s_argv.push_back(av); } \
    /* check help */ if (CHECK_FLAG(FLAGS, ARGC, ARGV)) { ap::s_help = true; AP_STDOUT << PTRNS(USAGE, "") << std::endl; PRINT_HELP(FLAGS, ap::s_help, MSG); } \
    /* parse value */ return ap::s_help;\
    }()

/*! \brief Define flag */
#define PARSE_FLAG(FLAGS, DEFAULT, MSG) [&](){\
    /* show help */ if (ap::s_help) { PRINT_HELP(FLAGS, DEFAULT, MSG); return DEFAULT; }\
    /* parse value */ return [&](){\
        /* check flag */ size_t j = [&]()->size_t { std::vector<std::string> flags; SEPARATE_FLAGS(FLAGS, flags); for (size_t i = 1; i < ap::s_argv.size(); ++i) for (size_t fi = 0; fi < flags.size(); ++fi) if (ap::s_argv[i] == flags[fi]) return i; return 0; }();\
        /* parse value */ auto value = DEFAULT; if (j) { if (typeid(DEFAULT) == typeid(bool)) { ap::s_argv.insert(ap::s_argv.begin() + j + 1, (*reinterpret_cast<bool*>(&value) ? "0" : "1")); } if ((++j) < ap::s_argv.size()) { std::string av = std::string(ap::s_argv[j]); std::istringstream iss(av); if (typeid(DEFAULT) == typeid(std::string)) value = *(reinterpret_cast<decltype(DEFAULT)*>(&av)); else iss >> value; ap::s_argv.erase(ap::s_argv.begin() + j); ap::s_argv.erase(ap::s_argv.begin() + j - 1); } }\
        /* return value */ return value;\
        }();\
    }()

/*! \brief Define argument */
#define PARSE_ARG(DEFAULT) [&](){\
    /* parse next argument */ auto arg = DEFAULT; if (ap::s_argv.size() > 1) { std::stringstream ss(ap::s_argv[1]); ss >> arg; ap::s_argv.erase(ap::s_argv.begin() + 1); } return arg;\
    }()

/*! \brief Add message */
#define ADD_MSG(MSG) [&](){ if (ap::s_help) AP_STDOUT << PTRNS(MSG, "") << std::endl; }()

/*! \brief Return number of unparsed arguments */
#define UNPARSED_COUNT() (ap::s_argv.size() - 1)

/*! \brief Check flags */
#define CHECK_FLAG(FLAGS, ARGC, ARGV) [&]()->bool { std::vector<std::string> flags; SEPARATE_FLAGS(FLAGS, flags); for (size_t j = 0; j < flags.size(); ++j) for (int i = 1; i < ARGC; ++i) if (flags[j] == std::string(ARGV[i])) return true; return false; }()

#if !defined(AP_STDOUT)
#define AP_STDOUT std::cout
#endif // !defined(AP_STDOUT)

/*** Helpers *****************************************************************/

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

namespace ap {

std::vector<std::string> s_argv;
bool s_help = false;
int s_alignment = 25;
std::string s_short_flag_prefixes = "";
std::string s_long_flag_delimiter = "=";

#define SEPARATE_FLAGS(FLAGS, ARRAY) [&](){ std::stringstream ss(FLAGS); std::string flag; while (std::getline(ss, flag, ',')) { TRIM_SPACES(flag); ARRAY.push_back(flag);} std::string& lastFlag = ARRAY.back(); size_t pos = lastFlag.find_last_of(" \t"); if (std::string::npos != pos) TRIM_SPACES(lastFlag.erase(pos)); }()
#define PRINT_HELP(FLAGS, DEFAULT, MSG) [&](){ std::stringstream defStream; defStream << DEFAULT; std::string flags = PTRNS(FLAGS, defStream.str()); int size = ap::s_alignment - std::string(flags).size() - 2; AP_STDOUT << "  " << flags; std::stringstream msgStream(PTRNS(MSG, defStream.str())); std::string msg; bool first = true; while (std::getline(msgStream, msg, '\n')) { AP_STDOUT << std::string(first ? (size > 1 ? size : 2) : ap::s_alignment, ' ') << msg.erase(0, std::min(msg.find_first_not_of(' '), msg.size())) << std::endl; first = false; } }()
#define REPLACE_PATTERN(MSG, PTRN, VALUE) [&](){ std::string str(MSG); std::string ptrn(PTRN); while (str.find(ptrn) < str.size()) str.replace(str.find(ptrn), ptrn.length(), std::string(VALUE)); return str; }()
#define PTRNS(STR, DEF) REPLACE_PATTERN(REPLACE_PATTERN(STR, "%p", ap::s_argv[0]), "%d", DEF)
#define TRIM_SPACES(STR) [&](){ size_t startpos = STR.find_first_not_of(" \t"); if (std::string::npos != startpos) STR.erase(0, startpos); size_t endpos = STR.find_last_not_of(" \t") + 1; if (std::string::npos != endpos) STR.erase(endpos); }()

} // namespace ap

#endif // ARG_PARSER_H
