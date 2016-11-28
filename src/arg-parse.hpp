#ifndef ARG_PARSE_HPP
#define ARG_PARSE_HPP

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

#include <initializer_list>
#include <map>
#include <string>
#include <vector>

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
    ArgParse(const OptionList& options);

    const Flag& def(const Flag& flag, const CallBackFunc func = nullptr);
    const Arg& def(const Arg& arg);

    const bool parse(const int argc, char* const argv[]);

    const std::string help();
    const std::string error();

    const bool check(const std::string& flagStr);
    template<typename T>
    const bool checkAndRead(const std::string& flagStr, T* value);

    Flag const& operator[](const std::string& idx);
    Flag const& operator[](const char* idx);

    Arg const& operator[](const std::size_t& idx);
    Arg const& operator[](const int idx);

    struct Counts {
        struct Params {
            const size_t all() const { return flags + args.all(); }
            size_t flags = 0u;
            struct {
                const size_t all() const { return required + nonRequired; }
                size_t required = 0u;
                size_t nonRequired = 0u;
            } args;
        } defined;
        struct {
            const size_t flags() const { return defined.flags + undefined.flags; }
            const size_t args() const { return defined.args.all() + undefined.args.all(); }
            const size_t all() const { return flags() + args(); }
            Params defined, undefined;
        } parsed;
    } counts;

    struct Options {
        struct { std::string name; } program = { "" };
        struct { bool strict; } mode = { false };
        struct Help {
            enum { ShowOnesWithDescription = 0, ShowAllDefined = 1, ShowAll = 2 };
            bool add;
            bool compact;
            size_t margin;
            int show;
            std::string tab;
        } help = { true, true, 0, Help::ShowAllDefined, "    " };
    } options;

    struct Errors {
        static const int kErrorGroupSize = 256;
        enum Codes {
            NoError = 0,
            Define = 1 * kErrorGroupSize,
                Define_FlagMultiply,
            Parse = 2 * kErrorGroupSize,
                Parse_RequiredFlagValueMissing,
                Parse_RequiredArgumentMissing,
                Parse_ArgVIsEmpty,
                Parse_ArgCBiggerThanElementsOfArgV,
            Option = 3 * kErrorGroupSize,
            Last
        };
        struct Error {
            const Codes code;
            const std::string message;
            struct Suspect {
                enum { GeneralType, FlagType, ArgType } const type;
                union {
                    const void* _ptr;
                    const Flag* flag;
                    const Arg* arg;
                };
            } const suspect;
        };

        std::vector<Error*> select(Codes code = Codes::Parse);
        std::vector<Error> data;
    } errors;

private:
    const Flag& addFlag(const Flag&, const CallBackFunc = nullptr);

    struct {
        std::map<std::string, Flag> data;
        std::map<std::string, Flag*> longs;
        std::map<std::string, Flag*> shorts;
    } _flags;
    std::vector<Arg> _args;
};

// Value

typedef std::initializer_list<std::string> ChooseList;

struct Value {
    static const bool Required; // = true

    Value(const Value&);
    Value(const std::string& defaultValue = "",
          const bool& require = !Required,
          const std::string& name = "",
          const std::string& description = "");
    Value(const ChooseList& chooseList,
          const bool& require = !Required,
          const std::string& name = "",
          const std::string& description = "");

    const bool empty() const { return str.empty(); }

    bool isRequired;
    bool isSet;
    std::string str;

// private:
    std::vector<std::string> _chooseList;
    std::string _name;
    std::string _description;
};

// Flag

struct Flag {
    static const Flag WrongFlag;

    Flag(const Flag&);
    Flag(const std::string& longFlag = "",
         const std::string& shortFlag = "",
         const std::string& description = "");
    Flag(const std::string& longFlag,
         const std::string& shortFlag,
         const std::string& description,
         const Value value);

    const bool isValid() const;

    bool isSet;
    bool isDefined;
    bool hasValue;
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

    Arg(const Arg&);
    Arg(const std::string& name = "",
        const std::string& description = "");
    Arg(const std::string& name,
        const std::string& description,
        const bool isRequired,
        const Value& defaultValue = Value());

    Arg(const Value& value);

    bool isDefined;
};

} // namespace argparse

#endif // ARG_PARSE_HPP
