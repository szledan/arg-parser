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
#include <list>
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

    /*!
     * \brief  Main constructor
     * \param interlacedOptions  options split by commas witout spaces
     *
     * * Examples
     * \code{.cpp}
     * ArgParse::ArgParse ap("program.name=my program name,tab=  ,help.show=2,help.add=false");
     * ap.options.program.name  == "my program name"; // Use this program name in generated help.
     * ap.options.tab           == "  "; // The 'tab' is two spaces.
     * ap.options.help.show     == 2; // The '2' means to ShowAll.
     * ap.options.help.add      == false; // Doesn't add to help flag with default.
     * \endcode
     *
     * \see ArgParse::Options
     */
    ArgParse(const std::string& interlacedOptions = "");
    /*!
     * \brief  Constructor with initializer list
     * \param options a list of option strings
     *
     * * Examples
     * \code{.cpp}
     * ArgParse::ArgParse ap( { "program.name=my program name",
     *                          "tab=\t",
     *                          "help.show=2",
     *                          "help.add=0" } );
     * ap.options.program.name  == "my program name"; // Use this program name in generated help.
     * ap.options.tab           == "\t"; // The 'tab' is a char of TAB.
     * ap.options.help.show     == 2; // The '2' means to ShowAll.
     * ap.options.help.add      == false; // Doesn't add to help flag with default.
     * \endcode
     *
     * \see ArgParse::Options
     */
    ArgParse(const OptionList& options);

    /*!
     * \brief  Add definitaion of a 'flag' (aka 'option')
     * \return  !!!
     */
    const Flag& def(const Flag&, const CallBackFunc = nullptr);
    /*!
     * \brief  !!!
     * \return  !!!
     */
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
    const Flag& addFlag(const Flag& flag, const CallBackFunc cbf = nullptr);
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
    static const bool Required; // = true

    Value(const Value& v);
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

    Flag(const Flag& f);
    Flag(const std::string& longFlag = "",
         const std::string& shortFlag = "",
         const std::string& description = "");
    Flag(const std::string& longFlag,
         const std::string& shortFlag,
         const std::string& description,
         const Value value);

    /*!
     * \brief Validate the flag.
     *
     * Rules:
     *  * Invalid a Flag if both _longFlag and _shortFlag strings are empty.
     *  * Valid a Flag if _longFlag size bigger than 2 and first two
     *    characters equal to the '-' char.
     *  * Valid a Flag if _shortFlag size equals to the 2 and first character
     *    is the '-'.
     *  * Otherwise the Flag is invalide.
     *
     * \return  result of validation
     */
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

/*!
 * \brief The Arg is an argument which isn't a Flag (aka option)
 */
struct Arg : Value {
    static const Arg WrongArg;

    Arg(const Arg& a);
    Arg(const std::string& name = "",
        const std::string& description = "",
        const bool isRequired = !Required,
        const Value& defaultValue = Value());
    Arg(const Value& value);

    bool isDefined;
};

} // namespace argparse

#endif // ARG_PARSE_H
