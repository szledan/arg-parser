#ifndef ARG_PARSER_HPP
#define ARG_PARSER_HPP

/* Copyright (C) 2016-2017, Szilard Ledan <szledan@gmail.com>
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

#include <assert.h>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <list>

namespace ap {

#ifdef AP_ASSERT
#undef AP_ASSERT
#endif // AP_ASSERT
#define AP_ASSERT(A) assert(A)

// Arg

struct Arg {
    static const bool Required = true;

    Arg(const Arg& a)
        : isSet(a.isSet)
        , isRequired(a.isRequired)
        , _str(a._str)
        , _default(a._default)
        , _name(a._name)
        , _help(a._help)
    {}

    Arg(const std::string& name = "")
        : _name(name)
    {}

    Arg& defRequired() { this->isRequired = true; return *this; }
    Arg& defNonRequired() { this->isRequired = false; return *this; }
    Arg& defDefault(const std::string& d) { this->_str = d; this->_default = d; return *this; }
    Arg& defHelp(const std::string& h) { this->_help = h; return *this; }
    Arg& defChooseList(const std::string& chl) { /* TODO */return *this; }

    const std::string& str() const { return isSet ? _str : _default; }
    void set(const std::string& str = "1")
    {
        this->isSet = true;
        this->_str = str;
    }

    bool isSet = false;
    bool isRequired = false;
// private:
    std::string _str = "";
    std::string _default = "";
    std::string _name = "";
    std::string _help = "";
};

// Flag

template<typename T>
inline bool strTo(const std::string& str, T* value)
{
    if (!value)
        return false;
    std::stringstream s;
    s << str;
    s >> (*value);
    return !s.fail();
}

struct Flag {
    static const Flag Wrong;

    Flag(const Flag& f)
        : isSet(f.isSet)
        , hasArg(f.hasArg)
        , arg(f.arg)
        , _name(f._name)
        , _alias(f._alias)
        , _help(f._help)
    {}

    Flag(const std::string& name = "", const std::string& alias = "") : _name(name), _alias(alias) {}

    Flag& defArg(const Arg& a) { this->hasArg = true; this->arg = a; return *this; }
    Flag& defHelp(const std::string& help) { this->_help = help; return *this; }
    Flag& defAlias(const std::string& a = "") { /* TODO */return *this; }


    template<typename E>
    typename std::enable_if<std::is_enum<E>::value, E>::type
    read() const
    {
        std::string v = arg._str;
        if (v.empty())
            v = arg._default;
        if (v.empty())
            return E(0);
        return E(1);
    }

    template<typename T>
    typename std::enable_if<!std::is_enum<T>::value, T>::type
    read() const
    {
        T v;
        if (!strTo(arg._str, &v) && !strTo(arg._default, &v))
            return T();
        return v;
    }

    void set() { this->isSet = true; }
    void set(const std::string& str) { this->isSet = true; this->arg.set(str); }

    const std::string& name() const { return _name; }

    bool isSet = false;
    bool hasArg = false;
    Arg arg;
// private:
    std::string _name = "";
    std::string _alias = "";
    std::string _help = "";
};

const Flag Flag::Wrong = Flag();


// ArgParser

class ArgParser {
    enum ParamType {
        ArgType = 0,
        ShortFlagType,
        ShortFlagsType,
        LongFlagWithEqType,
        LongFlagType,
    };

    const ParamType mapParamType(const std::string& arg)
    {
        if (arg.size() <= 1 || arg[0] != '-' || (arg.size() == 2 && arg[1] == '-'))
            return ParamType::ArgType;

        AP_ASSERT(arg[0] == '-');
        if (arg.size() == 2) {
            AP_ASSERT(arg[1] != '-');
            return ParamType::ShortFlagType;
        }

        AP_ASSERT(arg.size() > 2);
        if (arg[1] != '-')
            return ParamType::ShortFlagsType;

        if (arg.find('=') != std::string::npos)
            return ParamType::LongFlagWithEqType;

        return ParamType::LongFlagType;
    }

public:
    ArgParser() : _flags(new Flags()) {}
    ~ArgParser()
    {
        if (_flags)
            delete _flags;
    }

    Flag* add(Flag* flag)
    {
        if (!flag || flag->_name.empty())
            return nullptr;
        return _flags->add(flag);
    }
    Arg* add(Arg*);
    Flag& def(const Flag& flag)
    {
        return *_flags->def(flag);
    }

    Arg& def(const Arg&);

    bool parse(int argc, char* argv[])
    {
        for (int argi = 1; argi < argc; ++argi) {
            std::string arg(argv[argi]);
            std::string value(argi + 1 < argc ? argv[argi + 1] : "");

            switch (mapParamType(arg)) {
            case ParamType::ShortFlagsType:
                for (size_t i = 1; i < arg.size(); ++i)
                    argi += _flags->setFlag(std::string("-") + arg[i], value, i == arg.size() - 1);
                break;
            case ParamType::LongFlagWithEqType: {
                const size_t posEq = arg.find("=");
                value = arg.substr(posEq + 1);
                arg = arg.substr(0, posEq);
                // Fall through.
            }
            case ParamType::ShortFlagType:
            case ParamType::LongFlagType:
                argi += _flags->setFlag(arg, value);
                break;
            case ParamType::ArgType:
                break;
            default:
                AP_ASSERT(false && "Unknow param type!");
                break;
            }
        }
        return true;
    }

    std::string help()
    {
        std::stringstream help;
        return help.str();
    }

    Flag const& operator[](const std::string& idx)
    {
        Flag* flag = _flags->get(idx);
        if (!flag)
            return Flag::Wrong;
        return (*flag);
    }

    Flag const& operator[](const char* idx)
    {
        return ArgParser::operator[](std::string(idx));
    }

    Arg const& operator[](const size_t& idx);

private:
    // Flags
    struct Flags {
        typedef std::map<std::string, Flag*> FlagMap;

        Flag* get(const std::string& name)
        {
            // TODO: change auto to explicite type.
            auto const& flagIt = flagMap.find(name);
            if (flagIt == flagMap.end())
                return nullptr;
            return flagIt->second;
        }

        Flag* add(Flag* flag)
        {
            AP_ASSERT(flag);
            AP_ASSERT(flag->name() == flag->_name);
            Flag* stored = get(flag->_name);
            if (stored)
                return stored;
            flagMap[flag->_name] = flag;
            if (!flag->_alias.empty())
                flagMap[flag->_alias] = flag;
            return flag;
        }

        Flag* def(const Flag& flag)
        {
            AP_ASSERT(flag.name() == flag._name);
            Flag* stored = get(flag._name);
            if (stored)
                return stored;

            flagList.push_back(flag);
            stored = &(flagList.back());

            return add(stored);
        }

        int setFlag(const std::string name, const std::string& value, const bool setValue = true)
        {
            Flag* flag = get(name);
            if (!flag)
                flag = def(Flag(name));
            if (flag->hasArg) {
                if (setValue && !get(value)) {
                    flag->set(value);
                    return 1;
                }
                flag->set();
            } else
                flag->set("1");
            return 0;
        }

        FlagMap flagMap;
        std::list<Flag> flagList;
    }* _flags;

    struct Args {
    }* _args;
};

} // namespace ap

#endif // ARG_PARSER_HPP
