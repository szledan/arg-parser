#ifndef ARG_PARSE_H
#define ARG_PARSE_H

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

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <initializer_list>

namespace argparse {

typedef void (*CallBackFunc)(void);

struct Value;
struct Arg;
struct Flag;

// ArgParse

class ArgParse {
public:    
    typedef std::initializer_list<std::string> OptionList;
    struct Errors;

    ArgParse(const std::string& interlacedOptions = "");
    ArgParse(const OptionList&);

    const Flag& def(const Flag&, CallBackFunc = nullptr);
    const Arg& def(const Arg&);

    const bool parse(const int argc, char* const argv[]);

    const std::string help();
    const std::string error();
    const std::vector<Errors>& errors() const;

    const bool checkFlag(const std::string& flagStr);
    template<typename T>
    const bool checkFlagAndReadValue(const std::string& flagStr, T* value);

    Flag const& operator[](const std::string& idx);
    Flag const& operator[](const char* idx);
    Arg const& operator[](const std::size_t& idx);
    Arg const& operator[](const int idx);

    struct Counts {
        struct {
            size_t defined;
            size_t undefined;
        } flags, args;
    } counts = { { 0u, 0u, }, { 0u, 0u } };

    struct Options {
        struct { std::string name; } program = { "" };
        std::string tab = "    ";
        struct { bool strict; } mode = { false };
        struct Help {
            enum { ShowOnesWithDescription = 0, ShowAllDefined = 1, ShowAll = 2 };
            bool add;
            bool compact;
            int show;
        } help = { true, true, Help::ShowAllDefined };
    } options;

    struct Errors {
        enum Codes {
            NoError = 0,
            RequiredFlagValueMissing,
            RequiredArgumentMissing,
            ArgVIsEmpty,
            ArgCBiggerThanElementsOfArgV,
        } const code;
        const std::string message;
        struct Suspect {
            enum {
                GeneralType,
                FlagType,
                ArgType,
            } const type;
            union {
                const void* _ptr;
                const Flag* flag;
                const Arg* arg;
            };
        } const suspect;
    };

private:
    void addError(const Errors::Codes&, const std::string& errorMsg, const ArgParse::Errors::Suspect& = { Errors::Suspect::GeneralType, nullptr });
    void addError(const Errors::Codes&, const std::string& errorMsg, const void*);
    void addError(const Errors::Codes&, const std::string& errorMsg, const Flag*);
    void addError(const Errors::Codes&, const std::string& errorMsg, const Arg*);

    std::map<std::string, Flag> _flags;
    std::map<std::string, Flag*> _longFlags;
    std::map<std::string, Flag*> _shortFlags;
    std::vector<Arg> _args;
    std::vector<Errors> _errors;
};

inline std::ostream& operator<<(std::ostream& os, const ArgParse::Errors& err);

// Value

typedef std::initializer_list<std::string> ChooseList;

struct Value {
    Value(const Value& v);

    Value(const std::string& defaultValue = "",
          const std::string& name = "",
          const std::string& description = "");

    Value(const std::string& defaultValue,
          const ChooseList& chooseList,
          const std::string& name = "",
          const std::string& description = "");

    const bool empty() const { return str.empty(); }

    std::string str;

// private:
    const std::string _getChoosesStr(bool full = true) const;

    std::string _name;
    std::string _description;
    std::vector<std::string> _chooseList;
    bool _isValueNeeded;
};

// Flag

struct Flag {
    static const Flag WrongFlag;

    Flag(const Flag& f);

    Flag(const std::string& longFlag = "",
         const std::string& shortFlag = "",
         const std::string& description = "");

    Flag(const std::string& longFlag,
         const std::string& shortFlag,
         const std::string& description,
         const Value value);

    bool isSet;
    bool hasValue;
    bool defined;
    Value value;
// private:
    std::string _longFlag;
    std::string _shortFlag;
    std::string _description;
    CallBackFunc _callBackFunc;
};

// Arg

struct Arg : Value {
    static const Arg WrongArg;

    static const bool IsNeeded = true;

    Arg(const Arg& a);

    Arg(const std::string& name = "",
        const std::string& description = "",
        const bool isNeeded = false,
        const Value& defaultValue = Value());
    Arg(const Value& value);

    void setArg(const std::string& value);

    bool isSet;
    bool defined;
// private:
    bool _isArgNeeded;
    CallBackFunc _callBackFunc;
};

} // namespace argparse

#endif // ARG_PARSE_H
