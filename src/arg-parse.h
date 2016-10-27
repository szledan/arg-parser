#ifndef ARG_PARSE_H
#define ARG_PARSE_H

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

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <initializer_list>

namespace argparse {

// Value

typedef void (*CallBackFunc)(void);

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

    const bool hasValue() const { return !_str.empty(); }
    const std::string getChoosesStr(int size = 0) const
    {
        std::stringstream ss;
        std::string end("");
        if (size) {
            end = "|...";
        } else size = _chooseList.size();
        if (size) {
            for (int i = 0; i < size; ++i)
                ss << "|" << _chooseList[i];
        }
        return ss.str().substr(1) + end;
    }

    std::string _str;
    std::string _name;
    std::string _description;
    std::vector<std::string> _chooseList;
    bool _isValueNeeded;
};

// Arg

struct Arg : Value {
    static const bool IsNeeded = true;

    Arg(const Arg& a);

    Arg(const std::string& name = "",
        const std::string& description = "",
        const bool isNeeded = false,
        const Value& defaultValue = Value());

    void setArg(const std::string& str)
    {
        _isSet = true;
        _str = str;
    }

    bool _isSet;
    bool _isArgNeeded;
    CallBackFunc _callBackFunc;
};

// Flag

struct Flag {
    Flag(const Flag& f);

    Flag(const std::string& longFlag= "",
         const std::string& shortFlag = "",
         const std::string& description = "");

    Flag(const std::string& longFlag,
         const std::string& shortFlag,
         const std::string& description,
         const Value value);

    std::string _longFlag;
    std::string _shortFlag;
    std::string _description;
    Value _value;
    bool _isSet;
    bool _hasValue;
    CallBackFunc _callBackFunc;
};

// ArgPars

class ArgParse {
public:
    ArgParse(int argc, char* argv[]);

    void add(Arg arg);
    void add(Arg arg, CallBackFunc callBackFunc);

    void add(Flag flag);
    void add(Flag flag, CallBackFunc callBackFunc);

    bool parse();
    const std::string showHelp();

    const bool checkFlag(const std::string& longFlag)
    {
        return _flags[longFlag]._isSet;
    }

    template<typename T>
    const bool checkFlagAndReadValue(const std::string& longFlag, T* value)
    {
        if (!checkFlag(longFlag))
            return false;

        std::string& valueStr = _flags[longFlag]._value._str;
        std::stringstream s;
        s << valueStr;
        s >> (*value);
        return !s.fail();
    }

    std::string operator[](const std::size_t& idx)
    {
        return _args[idx]._str;
    }
private:
    const std::string showError(const std::string& errorArg);

    int _argc;
    char** _argv;
    std::string _programName;
    std::vector<Arg> _args;
    std::map<std::string, Flag> _flags;
};

} // namespace argparse

#endif // ARG_PARSE_H
