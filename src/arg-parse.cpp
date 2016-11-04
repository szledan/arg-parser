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

#include "arg-parse.h"

#include <assert.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace argparse {

// Util functions.

namespace {

const bool readOptionValue(const ArgParse::OptionList& optionList, ArgParse::Options::Option& opt)
{
    for(auto option : optionList) {
        const size_t posEq = option.find("=");
        if (posEq != std::string::npos) {
            if (option.substr(0, posEq) == opt.name) {
                ArgParse::Options::set(opt, option.substr(posEq + 1));
                break;
            }
        }
    }

    return opt.current.isSet();
}

enum ParamType {
    ArgType = 0,
    ShortFlagType,
    ShortFlagsType,
    LongFlagWithEqType,
    LongFlagWithoutEqType,
};

ParamType mapParamType(const std::string& arg)
{
    assert(arg.size());

    if (arg.size() == 1 || arg[0] != '-' || (arg.size() == 2 && arg[1] == '-'))
        return ParamType::ArgType;

    assert(arg[0] == '-');
    if (arg.size() == 2) {
        assert(arg[1] != '-');
        return ParamType::ShortFlagType;
    }

    assert(arg.size() > 2);
    if (arg[1] != '-') {
        return ParamType::ShortFlagsType;
    }

    if (arg.find('=') != std::string::npos) {
        return ParamType::LongFlagWithEqType;
    }

    return ParamType::LongFlagWithoutEqType;
}

const bool findValue(const std::string& valueStr, const std::vector<std::string>& chooseList)
{
    if (!chooseList.size())
        return true;

    for (auto const& choose : chooseList) {
        if (choose == valueStr)
            return true;
    }

    return false;
}

} // namespace anonymous

// ArgPars

ArgParse::ArgParse(const OptionList& oList)
    : _undefArgsCount(0)
    , _defArgsCount(0)
    , _undefFlagsCount(0)
    , _defFlagsCount(0)
{
    readOptionValue(oList, options.programName);
    readOptionValue(oList, options.tab);
    readOptionValue(oList, options.mode);
    if (readOptionValue(oList, options.helpFlag))
        add(Flag("--help", "-h", "Show this help."));
}

const Arg& ArgParse::add(const Arg& arg)
{
    _args.push_back(arg);
    return _args.back();
}

const Flag& ArgParse::add(const Flag& flag, CallBackFunc cbf)
{
    std::string name = flag._shortFlag + flag._longFlag;

    _flags[name] = flag;

    Flag* flagPtr = &(_flags[name]);
    if (!flag._longFlag.empty()) {
        _longFlags[flag._longFlag] = flagPtr;
    }
    if (!flag._shortFlag.empty()) {
        _shortFlags[flag._shortFlag] = flagPtr;
    }

    flagPtr->_callBackFunc = cbf;
    return *flagPtr;
}

const bool ArgParse::parse(const int argc_, char* const argv_[])
{
    if ((argc_ < 1) || (!argv_) || (!argv_[0])) {
        addError(ErrorARGVEmpty, "Wrong argument count: 0!");
        return false;
    }
    size_t argc = (size_t)argc_;

    // Set program name.
    if (!options.programName.current.isSet()) {
        assert(options.programName.current.value.empty());
        Options::set(options.programName, std::string(argv_[0]));
    }

    // Calculate number of required arguments.
    int requiredArgs = 0;
    for (auto const& arg : _args)
        if (arg._isArgNeeded)
            requiredArgs++;

    std::vector<CallBackFunc> callBackFuncs;
    // Parse params.
    for (size_t adv = 1, argCount = 0; adv < argc; ++adv) {
        std::string paramStr(argv_[adv]);
        std::string valueStr(adv + 1 < argc ? argv_[adv + 1] : "");

#define AP_SETUP_FLAG(FLAGS, CHECH_VALUE) do { \
        if (FLAGS.find(paramStr) == FLAGS.end()) { \
            add(Flag(paramStr, "")); \
            _undefFlagsCount++; \
        } else \
            _defFlagsCount++; \
    \
        Flag* flag = FLAGS[paramStr]; \
        assert(flag); \
        flag->isSet = true; \
        if (flag->_callBackFunc) { \
            callBackFuncs.push_back(flag->_callBackFunc); \
        } \
    \
        if ((CHECH_VALUE) && flag->hasValue) { \
            if (valueStr.empty() && flag->value._isValueNeeded) { \
                addError(ErrorRequiredFlagValueMissing, "", flag); \
            } else if ((flag->value._isValueNeeded) \
                       && (mapParamType(valueStr) != ParamType::ArgType) \
                       && (checkFlag(valueStr))) { \
                addError(ErrorRequiredFlagValueMissing, "", flag); \
            } else if (!findValue(valueStr, flag->value._chooseList)) { \
                    addError(ErrorRequiredFlagValueMissing, "", flag); \
            } else { \
                flag->value.str = valueStr; \
                ++adv; \
            } \
        } \
    } while (false)

        switch (mapParamType(paramStr)) {
        case ParamType::ArgType:
            if (_args.size() > argCount) {
                if (_args[argCount]._isArgNeeded)
                    requiredArgs--;
                _args[argCount].setArg(paramStr);
                _defArgsCount++;
            } else {
                _args.push_back(Arg("", "", false, Value(paramStr)));
                _undefArgsCount++;
            }
            argCount++;
            break;
        case ParamType::ShortFlagsType: {
            const std::string shortFlags = paramStr;
            for (size_t i = 1; i < paramStr.size(); ++i) {
                paramStr = std::string("-") + shortFlags[i];
                const bool check = i == (paramStr.size() - 1);
                AP_SETUP_FLAG(_shortFlags, check);
            }
            break;
        }
        case ParamType::ShortFlagType:
            AP_SETUP_FLAG(_shortFlags, true);
            break;
        case ParamType::LongFlagWithEqType: {
            const size_t posEq = paramStr.find("=");
            valueStr = paramStr.substr(posEq + 1);
            paramStr = paramStr.substr(0, posEq);
            // Fall through.
        }
        case ParamType::LongFlagWithoutEqType:
            AP_SETUP_FLAG(_longFlags, true);
            break;
        default:
            assert(false);
            break;
        };
#undef AP_SETUP_FLAG
    }

    if (requiredArgs) {
        for (int i = requiredArgs; i; --i)
            addError(ErrorRequiredArgumentMissing, "Required argument missing!", &_args[_args.size() - i]);
        return false;
    }

    // Call functions.
    if (!_errors.size()) {
        for (auto const& func : callBackFuncs)
            func();
    }

    return !_errors.size();
}

const std::string ArgParse::help()
{
    const std::string tab = options.tab.current.isSet() ? options.tab.current.value : options.tab.init.value;
    std::stringstream help;

    // Print program name.
    help << "usage: " << options.programName.current.value;

    // Print arguments after programname.
    for (auto const& it : _args) {
        const Arg& arg = it;
        if (!arg._name.empty()) {
            if (arg._isArgNeeded)
                help << " <" << arg._name << "> ";
            else
                help << " [<" << arg._name << ">] ";
        }
    }

    // Print arguments.
    help << std::endl << std::endl << "Arguments:" << std::endl;

    for (auto const& it : _args) {
        const Arg& arg = it;
        if (!arg._name.empty()) {
            if (arg._isArgNeeded)
                help << tab << " <" << arg._name << "> ";
            else
                help << tab << " [<" << arg._name << ">] ";
            if (!arg._description.empty())
                help << tab << arg._description;
            help << std::endl;
        }
    }

    // Print option flags.
    help << std::endl << "Option flags:" << std::endl;

    for (auto const& it : _flags) {
#define AP_HAS_NAME(CH, VALNAME) do { \
        if (flag.value._isValueNeeded) \
            help << CH << "<" << VALNAME << ">"; \
        else \
            help << CH << "[<" << VALNAME << ">]"; \
    } while(false)

#define AP_PRINT_FLAG(FLAG, L) do { \
        help << FLAG; \
        if (flag.value._chooseList.size() > 0) \
            AP_HAS_NAME(" ", flag.value._getChoosesStr(hasLongFlag ? L : (L ? 0 : 1))); \
        else if (!flag.value._name.empty()) \
            AP_HAS_NAME(" ", flag.value._name); \
    } while(false)

        const Flag& flag = it.second;
        const bool hasShortFlag = !flag._shortFlag.empty();
        const bool hasLongFlag = !flag._longFlag.empty();

        help << tab;
        if (hasShortFlag)
            AP_PRINT_FLAG(flag._shortFlag, 1);
        if (hasLongFlag) {
            if (hasShortFlag)
                help << ", ";
            AP_PRINT_FLAG(flag._longFlag, 0);
        }
        if (hasShortFlag || hasLongFlag) {
            help << tab << flag._description << std::endl;
        }

#undef AP_PRINT_FLAG
#undef AP_HAS_NAME
    }

    return help.str();
}

inline std::ostream& operator<<(std::ostream& os, const ArgParse::ArgError& err)
{
    std::string typeSpecMsg("");
    if (err.type == ArgParse::ArgError::ArgType)
        typeSpecMsg = std::string(", arg: ") + err.arg->_name;
    else if (err.type == ArgParse::ArgError::FlagType)
        typeSpecMsg = std::string(", flag: ") + err.flag->_longFlag + " " + err.flag->_shortFlag;

    os << "error: '" << err.errorMessage << "', code: " << err.errorCode << typeSpecMsg << ".";
    return os;
}

const std::string ArgParse::error()
{
    std::stringstream error;
    for (auto const& err : _errors)
        error << err << std::endl;
    return error.str();
}

const bool ArgParse::checkFlag(const std::string& flagStr)
{
    return (_longFlags.find(flagStr) != _longFlags.end()) || (_shortFlags.find(flagStr) != _shortFlags.end());
}

template<typename T>
const bool ArgParse::checkFlagAndReadValue(const std::string& flagStr, T* value)
{
    if (!checkFlag(flagStr))
        return false;

    const Flag& flag = _flags[flagStr];
    if (!flag.hasValue)
        return false;

    std::stringstream s;
    s << flag.value.str;
    s >> (*value);
    return !s.fail();
}

Arg const& ArgParse::operator[](const std::size_t& idx)
{
    return _args[idx];
}

const Flag& ArgParse::operator[](const std::string& idx)
{
    std::string flagStr(idx);
    switch (mapParamType(flagStr)) {
    case ParamType::ArgType:
        assert(false);
        break;
    case ParamType::ShortFlagsType:
        flagStr = flagStr.substr(0, 2);
        // Fall through.
    case ParamType::ShortFlagType:
        if (_shortFlags.find(flagStr) == _shortFlags.end())
            return add(Flag("", flagStr));
        return *(_shortFlags[flagStr]);
    case ParamType::LongFlagWithEqType:
        flagStr = flagStr.substr(0, flagStr.find("="));
        // Fall through.
    case ParamType::LongFlagWithoutEqType:
        if (_longFlags.find(flagStr) == _longFlags.end())
            return add(Flag(flagStr));
        return *(_longFlags[flagStr]);
    default:
        assert(false);
        break;
    };

    return _flags[""];
}

Flag const& ArgParse::operator[](const char* idx)
{
    return ArgParse::operator[](std::string(idx));
}

void ArgParse::Options::set(ArgParse::Options::Option& opt, const std::string& value, const int& state)
{
    int currentState = state;

    if (state == Option::Value::Unused && opt.init.state != Option::Value::Unused) {
        if (value == opt.init.value) {
            currentState = opt.init.state;
        } else {
            if (opt.init.state < Option::Value::NotBool) {
                assert(((!Option::Value::Set) == Option::Value::NotSet) && ((!Option::Value::NotSet) == Option::Value::Set));
                currentState = !opt.init.state;
            } else {
                currentState = opt.current.state;
            }
        }
    }

    opt.current.state = currentState;
    opt.current.value = value;
}

void ArgParse::addError(const ArgParse::ErrorCodes& errorCode, const std::string& errorMsg)
{
    const ArgError ae = { errorCode, ArgError::GeneralType, nullptr, errorMsg };
    _errors.push_back(ae);
}

void ArgParse::addError(const ArgParse::ErrorCodes& errorCode, const std::string& errorMsg, const Arg* arg)
{
    const ArgError ae = { errorCode, ArgError::ArgType, reinterpret_cast<const void*>(arg), errorMsg };
    _errors.push_back(ae);
}

void ArgParse::addError(const ArgParse::ErrorCodes& errorCode, const std::string& errorMsg, const Flag* flag)
{
    const ArgError ae = { errorCode, ArgError::FlagType, reinterpret_cast<const void*>(flag), errorMsg };
    _errors.push_back(ae);
}

// Value

Value::Value(const Value& v)
    : str(v.str)
    , _name(v._name)
    , _description(v._description)
    , _chooseList(v._chooseList)
    , _isValueNeeded(v._isValueNeeded)
{
}

Value::Value(const std::string& defaultValue, const std::string& name, const std::string& description)
    : str(defaultValue)
    , _name(name)
    , _description(description)
    , _isValueNeeded(defaultValue.empty())
{
}

Value::Value(const std::string& defaultValue, const ChooseList& chooseList, const std::string& name, const std::string& description)
    : Value(defaultValue, name, description)
{
    for(auto choose : chooseList)
        _chooseList.push_back(choose);
}

const std::string Value::_getChoosesStr(const bool full) const
{
    if (!_chooseList.size())
        return "";

    std::stringstream ss;
    std::string end(full ? "" : "|...");

    for (size_t i = 0; i < _chooseList.size(); ++i)
        ss << "|" << _chooseList[i];

    return ss.str().substr(1) + end;
}

// Arg

Arg::Arg(const Arg& a)
    : Value((Value)a)
    , isSet(a.isSet)
    , _isArgNeeded(a._isArgNeeded)
    , _callBackFunc(a._callBackFunc)
{
}

Arg::Arg(const std::string& name,
         const std::string& description,
         const bool isNeeded,
         const Value& defaultValue)
    : Value(defaultValue.str, name, description)
    , isSet(false)
    , _isArgNeeded(isNeeded)
    , _callBackFunc(nullptr)
{
}

Arg::Arg(const Value& value)
    : Arg("", "", false, value)
{
}

void Arg::setArg(const std::string& value)
{
    isSet = true;
    str = value;
}

// Flag

Flag::Flag(const Flag& f)
    : isSet(f.isSet)
    , value(f.value)
    , _longFlag(f._longFlag)
    , _shortFlag(f._shortFlag)
    , _description(f._description)
    , _callBackFunc(f._callBackFunc)
{
}

Flag::Flag(const std::string& lFlag,
           const std::string& sFlag,
           const std::string& dscrptn)
    : isSet(false)
    , hasValue(false)
    , _longFlag(lFlag)
    , _shortFlag(sFlag)
    , _description(dscrptn)
    , _callBackFunc(nullptr)
{
}

Flag::Flag(const std::string& lFlag,
           const std::string& sFlag,
           const std::string& dscrptn,
           const Value definedValue)
    : Flag(lFlag, sFlag, dscrptn)
{
    hasValue = true;
    value = definedValue;
}

} // namespace argparse
