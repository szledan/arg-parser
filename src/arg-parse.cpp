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

#if 0
/*
 * TODO LIST:
 *  [1] More more tests :)
 *  [2] Implement 'mode.strict', and 'help.lexicographic'
 *  [5] Replace every 'true/false' to const values.
 *
 * FUTURE LIST:
 *  [6] Extend ArgParse with: argc(), argv() native getter functions to _argc, _argv.
 *  [7] Extend './demos' with some typical example.
 */
#endif

#include "arg-parse.hpp"

#include <assert.h>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace argparse {

// Util functions.
namespace {

template <typename T>
const T returnValue(const std::string& value, const T& def)
{
    std::stringstream s;
    T retValue;
    s << value;
    s >> retValue;
    return !s.fail() ? retValue : def;
}

const bool returnValue(const std::string& value, const bool& def)
{
    return !(value.empty() || (value == "false") || (value == "off") || (value == "0"));
}

const std::string returnValue(const std::string& value, const std::string& def)
{
    return value;
}

template <typename T>
const T readOption(const std::string& oList, const std::string& key, const T& def)
{
    if (oList.empty())
        return def;

    std::stringstream optionList(oList);
    std::string option;
    while (std::getline(optionList, option, ',')) {
        const size_t posEq = option.find("=");
        if (posEq != std::string::npos) {
            if (option.substr(0, posEq) == key)
                return returnValue(option.substr(posEq + 1), def);
        }
    }

    return def;
}

const std::string interlacedOptions(const ArgParse::OptionList& oList)
{
    std::string interlacedStr;
    for(auto option : oList)
        interlacedStr += option + ',';
    return interlacedStr;
}

enum ParamType {
    ArgType = 0,
    ShortFlagType,
    ShortFlagsType,
    LongFlagWithEqType,
    LongFlagWithoutEqType,
};

const ParamType mapParamType(const std::string& arg)
{
    if (arg.size() <= 1 || arg[0] != '-' || (arg.size() == 2 && arg[1] == '-'))
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

const std::string getChoosesStr(const Value& value, const bool isShort)
{
    if (!value._chooseList.size())
        return "";

    std::stringstream s;
    std::string end(isShort ? "|..." : "");
    const size_t size = isShort ? 1 : value._chooseList.size();
    for (size_t i = 0; i < size; ++i)
        s << "|" << value._chooseList[i];

    return s.str().substr(1) + end;
}

const std::string& cutMargin(std::string* margin, const std::string& printStr)
{
    margin->erase(0, printStr.size());
    return printStr;
}

const std::string generateStr(const size_t& size, const std::string& str)
{
    std::string generated("");
    for (size_t i = 0; i < size; ++i)
        generated.append(str);
    return generated;
}

void setArg(Arg& arg, const std::string& value)
{
    arg.isSet = true;
    arg.str = value;
}

void addError(std::vector<ArgParse::Errors>* errors, const ArgParse::Errors::Codes& errorCode, const std::string& errorMsg, const ArgParse::Errors::Suspect& suspect)
{
    const ArgParse::Errors ae = { errorCode, errorMsg, suspect };
    errors->push_back(ae);
}

void addError(std::vector<ArgParse::Errors>* errors, const ArgParse::Errors::Codes& errorCode, const std::string& errorMsg, const void* ptr)
{
    addError(errors, errorCode, errorMsg, { ArgParse::Errors::Suspect::GeneralType, ptr });
}

void addError(std::vector<ArgParse::Errors>* errors, const ArgParse::Errors::Codes& errorCode, const std::string& errorMsg, const Flag* flag)
{
    addError(errors, errorCode, errorMsg, { ArgParse::Errors::Suspect::FlagType, reinterpret_cast<const void*>(flag) });
}

void addError(std::vector<ArgParse::Errors>* errors, const ArgParse::Errors::Codes& errorCode, const std::string& errorMsg, const Arg* arg)
{
    addError(errors, errorCode, errorMsg, { ArgParse::Errors::Suspect::ArgType, reinterpret_cast<const void*>(arg) });
}

} // namespace anonymous

// ArgPars

ArgParse::ArgParse(const std::string& oList)
{
#define AP_SET_OPTION(VALUE, KEY) (VALUE = readOption(oList, KEY, VALUE))
    AP_SET_OPTION(options.program.name, "program.name");
    AP_SET_OPTION(options.mode.strict, "mode.strict");
    if (AP_SET_OPTION(options.help.add, "help.add"))
        def(Flag("--help", "-h", "Show this help."));
    AP_SET_OPTION(options.help.compact, "help.compact");
    AP_SET_OPTION(options.help.margin, "help.margin");
    AP_SET_OPTION(options.help.show, "help.show");
    AP_SET_OPTION(options.help.tab, "help.tab");
#undef AP_SET_OPTION
}

ArgParse::ArgParse(const OptionList& oList)
    : ArgParse(interlacedOptions(oList))
{
}

const Flag& ArgParse::addFlag(const Flag& flag, const CallBackFunc cbf)
{
    const std::string name = flag._longFlag + "-" + flag._shortFlag;
    assert(!name.empty());

    Flag* flagPtr = &(_flags[name] = flag);
    assert(flagPtr == &(_flags[name]));

    if (!flag._longFlag.empty()) {
        _longFlags[flag._longFlag] = flagPtr;
    }
    if (!flag._shortFlag.empty()) {
        _shortFlags[flag._shortFlag] = flagPtr;
    }

    flagPtr->_callBackFunc = cbf;
    return *flagPtr;
}

const Flag& ArgParse::def(const Flag& flag, const CallBackFunc cbf)
{
    if (!flag.isDefined || !flag.isValid())
        return Flag::WrongFlag;

    const bool hasLongFlag = check(flag._longFlag);
    const bool hasShortFlag = check(flag._shortFlag);
    if (hasLongFlag || hasShortFlag) {
        const Flag* addedFlag = hasLongFlag ? _longFlags[flag._longFlag] : _shortFlags[flag._shortFlag];
        addError(&_errors, Errors::FlagMultiplyDefination, "Multiply definations of flags", addedFlag);
        return *addedFlag;
    }

    counts.defined.flags++;
    return addFlag(flag, cbf);
}

const Arg& ArgParse::def(const Arg& arg)
{
    if (!arg.isDefined)
        return Arg::WrongArg;
    if (arg.isRequired)
        counts.defined.args.required++;
    else
        counts.defined.args.nonRequired++;
    _args.push_back(arg);
    return _args.back();
}

const bool ArgParse::parse(const int argc_, char* const argv_[])
{
    if ((argc_ < 1) || (!argv_) || (!argv_[0])) {
        addError(&_errors, Errors::ArgVIsEmpty, "Wrong argument count: 0!", argv_ ? reinterpret_cast<void*>(argv_[0]) : nullptr);
        return false;
    }
    for (int argIndex = 0; argIndex < argc_; ++argIndex) {
        if (argv_[argIndex] == nullptr) {
            std::string message = std::string("The 'argc' > count('argv'): ") + std::to_string(argc_) + " != " + std::to_string(argIndex);
            addError(&_errors, Errors::ArgCBiggerThanElementsOfArgV, message, reinterpret_cast<void*>(argv_[0]));
            return false;
        }
    }
    const size_t argc = (size_t)argc_;

    // Set program name.
    if (options.program.name.empty())
        options.program.name = std::string(argv_[0]);

    // Calculate number of required arguments.
    int requiredArgs = 0;
    for (auto const& arg : _args)
        if (arg.isRequired)
            requiredArgs++;

    std::vector<CallBackFunc> callBackFuncs;
    // Parse params.
    for (size_t adv = 0, argCount = 0; adv < argc; ++adv) {
        struct {
            std::string paramStr;
            const ParamType paramType;
            const bool hasNextParam;
            std::string valueStr;
        } param = {
            std::string(argv_[adv]),
            mapParamType(param.paramStr),
            adv + 1 < argc,
            std::string(param.hasNextParam ? argv_[adv + 1] : ""),
        };

#define AP_IS_LONG (true)
#define AP_CHECH_VALUE (true)
#define AP_CHECK_FLAG_EXIST(FLAGS, IS_LONG) do { \
        if (FLAGS.find(param.paramStr) == FLAGS.end()) { \
            if (IS_LONG) \
                addFlag(Flag(param.paramStr, "")); \
            else \
                addFlag(Flag("", param.paramStr)); \
            FLAGS[param.paramStr]->isDefined = false; \
            counts.parsed.undefined.flags++; \
        } else \
            counts.parsed.defined.flags++; \
    } while (false)

#define AP_SETUP_FLAG(FLAGS, CHECH_VALUE) do { \
        Flag* flag = FLAGS[param.paramStr]; \
        assert(flag); \
        flag->isSet = true; \
        if (flag->_callBackFunc) { \
            callBackFuncs.push_back(flag->_callBackFunc); \
        } \
    \
        if ((CHECH_VALUE) && flag->hasValue) { \
            if (!param.hasNextParam && flag->value.isRequired) { \
                addError(&_errors, Errors::RequiredFlagValueMissing, "Missing required value.", flag); \
            } else if ((flag->value.isRequired) \
                       && (mapParamType(param.valueStr) != ParamType::ArgType) \
                       && (check(param.valueStr))) { \
                addError(&_errors, Errors::RequiredFlagValueMissing, "Missing required value, next is a defined flag.", flag); \
            } else if (!findValue(param.valueStr, flag->value._chooseList)) { \
                    addError(&_errors, Errors::RequiredFlagValueMissing, "Did not find choose in list.", flag); \
            } else { \
                flag->value.str = param.valueStr; \
                if (param.paramType != LongFlagWithEqType) \
                    ++adv; \
            } \
        } \
    } while (false)

        switch (param.paramType) {
        case ParamType::ShortFlagsType: {
            const std::string shortFlags = param.paramStr;
            for (size_t i = 1; i < shortFlags.size(); ++i) {
                param.paramStr = std::string("-") + shortFlags[i];
                const bool checkValue = i == (param.paramStr.size() - 1);
                AP_CHECK_FLAG_EXIST(_shortFlags, !AP_IS_LONG);
                AP_SETUP_FLAG(_shortFlags, checkValue);
            }
            break;
        }
        case ParamType::ShortFlagType:
            AP_CHECK_FLAG_EXIST(_shortFlags, !AP_IS_LONG);
            AP_SETUP_FLAG(_shortFlags, AP_CHECH_VALUE);
            break;
        case ParamType::LongFlagWithEqType: {
            const bool isDefinedFullParam = _longFlags.find(param.paramStr) != _longFlags.end();
            if (!isDefinedFullParam) {
                const size_t posEq = param.paramStr.find("=");
                param.valueStr = param.paramStr.substr(posEq + 1);
                param.paramStr = param.paramStr.substr(0, posEq);

                if (_longFlags.find(param.paramStr) == _longFlags.end()) {
                    // Add undefined flag with value.
                    def(Flag(param.paramStr, "", "", Value("_")));
                    _longFlags[param.paramStr]->value.str = param.valueStr;
                    counts.parsed.undefined.flags++;
                } else {
                    AP_CHECK_FLAG_EXIST(_longFlags, AP_IS_LONG);
                }
            }
            AP_SETUP_FLAG(_longFlags, AP_CHECH_VALUE);
            break;
        }
        case ParamType::LongFlagWithoutEqType:
            AP_CHECK_FLAG_EXIST(_longFlags, AP_IS_LONG);
            AP_SETUP_FLAG(_longFlags, AP_CHECH_VALUE);
            break;
        case ParamType::ArgType:
            if (_args.size() > argCount) {
                if (_args[argCount].isRequired) {
                    counts.parsed.defined.args.required++;
                    requiredArgs--;
                } else
                    counts.parsed.defined.args.nonRequired++;
                setArg(_args[argCount], param.paramStr);
            } else {
                counts.parsed.undefined.args.nonRequired++;
                _args.push_back(Arg("", "", !Arg::Required, Value(param.paramStr)));
            }
            argCount++;
            break;
        default:
            assert(false);
            break;
        };
#undef AP_SETUP_FLAG
#undef AP_CHECK_FLAG_EXIST
#undef AP_CHECH_VALUE
#undef AP_IS_LONG
    }

    if (requiredArgs) {
        for (int i = requiredArgs; i; --i)
            addError(&_errors, Errors::RequiredArgumentMissing, "Required argument missing!", &_args[_args.size() - i]);
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
    const std::string tab = options.help.tab;
    std::stringstream help;

    // Print program name.
    help << "usage: " << options.program.name;

    // Add '[options]' for usage part.
    if (!_flags.empty())
        help << " [options]";

    // Print arguments after programname.
    for (auto const& it : _args) {
        const Arg& arg = it;
        if (!arg._name.empty()) {
            if (arg.isRequired)
                help << " <" << arg._name << "> ";
            else
                help << " [<" << arg._name << ">] ";
        }
    }

#define AP_PRINT_NAME(CH, VALUE, VALNAME) do { \
        std::stringstream s; \
        if (VALUE.isRequired) \
            s << CH << "<" << VALNAME << ">"; \
        else \
            s << CH << "[<" << VALNAME << ">]"; \
        help << cutMargin(&margin, s.str()); \
    } while (false)

#define AP_PRINT_DESCRIPTION(HELP, DESCRIPT, MARGIN_SIZE, MARGIN_STR, TAB) do { \
        std::string line; \
        while (std::getline(DESCRIPT, line, '\n')) { \
            if (!MARGIN_SIZE) \
                MARGIN_STR += TAB; \
            HELP << MARGIN_STR << line << std::endl; \
            MARGIN_STR = MARGIN_SIZE ? generateStr(MARGIN_SIZE, " ") : TAB; \
        } \
    } while (false)

    // Print arguments.
    help << std::endl << std::endl << "Arguments:" << std::endl;

    for (auto const& arg : _args) {
        if (!arg._name.empty()
            && ((options.help.show == Options::Help::ShowOnesWithDescription && !arg._description.empty())
                || (options.help.show == Options::Help::ShowAllDefined && arg.isDefined)
                || options.help.show == Options::Help::ShowAll)) {
            std::string margin(generateStr(options.help.margin, " "));
            help << cutMargin(&margin, tab);
            AP_PRINT_NAME("", arg, arg._name);
            if (!arg._description.empty()) {
                std::stringstream description;
                if (!options.help.compact || (options.help.margin && margin.empty()))
                    description << std::endl;
                description << arg._description;
                if (arg._chooseList.size() && !arg._name.empty()) {
                    description << std::endl << arg._name;
                    description << "={" << getChoosesStr(arg, false) << "}";
                }

                AP_PRINT_DESCRIPTION(help, description, options.help.margin, margin, tab);
            } else
                help << std::endl;
        }
    }

    // Print option flags.
    help << std::endl << "Option flags:" << std::endl;

    for (auto const& it : _flags) {
#define AP_PRINT_FLAG(FLAG, L) do { \
        help << FLAG; \
        if (flag.hasValue) {\
            if (flag.value._chooseList.size()) \
                AP_PRINT_NAME(" ", flag.value, (!flag.value._name.empty() ? flag.value._name : getChoosesStr(flag.value, hasLongFlag ? L : (L ? 0 : 1)))); \
            else \
                AP_PRINT_NAME(" ", flag.value, (flag.value._name.empty() ? "..." : flag.value._name)); \
        } \
    } while (false)

        const Flag& flag = it.second;
        const bool hasShortFlag = !flag._shortFlag.empty();
        const bool hasLongFlag = !flag._longFlag.empty();
        if ((options.help.show == Options::Help::ShowOnesWithDescription && flag._description.empty())
            || (options.help.show == Options::Help::ShowAllDefined && !flag.isDefined))
            continue;

        std::string margin(generateStr(options.help.margin, " "));
        if (hasShortFlag || hasLongFlag)
            help << cutMargin(&margin, tab);
        if (hasShortFlag)
            AP_PRINT_FLAG(cutMargin(&margin, flag._shortFlag), 1);
        if (hasLongFlag) {
            if (hasShortFlag)
                help << cutMargin(&margin, ", ");
            AP_PRINT_FLAG(cutMargin(&margin, flag._longFlag), 0);
        }

        if (hasShortFlag || hasLongFlag) {
            std::stringstream description;
            if (!options.help.compact || (options.help.margin && margin.empty()))
                description << std::endl;
            if (!flag._description.empty()) {
                description << flag._description;
            }
            if (flag.value._chooseList.size() && !flag.value._name.empty()) {
                description << std::endl << flag.value._name;
                description << "={" << getChoosesStr(flag.value, false) << "}";
                description << tab << flag.value._description;
            }
            if (!options.help.compact)
                description << std::endl << std::endl;

            AP_PRINT_DESCRIPTION(help, description, options.help.margin, margin, tab);
        }
#undef AP_PRINT_FLAG
    }
#undef AP_PRINT_DESCRIPTION
#undef AP_PRINT_NAME

    return help.str();
}

inline std::ostream& operator<<(std::ostream& os, const ArgParse::Errors& err)
{
    std::string typeSpecMsg("");
    if (err.suspect.type == ArgParse::Errors::Suspect::FlagType)
        typeSpecMsg = std::string(", flag: ") + err.suspect.flag->_longFlag + " " + err.suspect.flag->_shortFlag;
    else if (err.suspect.type == ArgParse::Errors::Suspect::ArgType)
        typeSpecMsg = std::string(", arg: ") + err.suspect.arg->_name;

    os << "error: '" << err.message << "', code: " << err.code << typeSpecMsg << ".";
    return os;
}

const std::string ArgParse::error()
{
    std::stringstream error;
    for (auto const& err : _errors)
        error << err << std::endl;
    return error.str();
}

const std::vector<ArgParse::Errors>&ArgParse::errors() const
{
    return _errors;
}

const bool ArgParse::check(const std::string& flagStr)
{
    return (_longFlags.find(flagStr) != _longFlags.end()) || (_shortFlags.find(flagStr) != _shortFlags.end());
}

template<typename T>
const bool ArgParse::checkAndRead(const std::string& flagStr, T* value)
{
    if (!check(flagStr) || !value)
        return false;

    const Flag& flag = _longFlags.find(flagStr) != _longFlags.end() ? *_longFlags[flagStr] : *_shortFlags[flagStr];
    if (!flag.hasValue)
        return false;

    std::stringstream s;
    s << flag.value.str;
    s >> (*value);
    return !s.fail();
}

const Flag& ArgParse::operator[](const std::string& idx)
{
    std::string flagStr(idx);
    switch (mapParamType(flagStr)) {
    case ParamType::ArgType:
        return Flag::WrongFlag;
    case ParamType::ShortFlagsType:
        flagStr = flagStr.substr(0, 2);
        // Fall through.
    case ParamType::ShortFlagType:
        if (_shortFlags.find(flagStr) == _shortFlags.end())
            return def(Flag("", flagStr));
        return *(_shortFlags[flagStr]);
    case ParamType::LongFlagWithEqType:
        if (_longFlags.find(flagStr) == _longFlags.end())
            flagStr = flagStr.substr(0, flagStr.find("="));
        // Fall through.
    case ParamType::LongFlagWithoutEqType:
        if (_longFlags.find(flagStr) == _longFlags.end())
            return def(Flag(flagStr));
        return *(_longFlags[flagStr]);
    default:
        assert(false);
        break;
    };

    return Flag::WrongFlag;
}

Flag const& ArgParse::operator[](const char* idx)
{
    return ArgParse::operator[](std::string(idx));
}

const Arg& ArgParse::operator[](const std::size_t& idx)
{
    return idx < _args.size() ? _args[idx] : Arg::WrongArg;
}

const Arg& ArgParse::operator[](const int idx)
{
    return operator[](std::size_t(idx));
}

// Value
const bool Value::Required = true;

Value::Value(const Value& v)
    : isRequired(v.isRequired)
    , isSet(v.isSet)
    , str(v.str)
    , _chooseList(v._chooseList)
    , _name(v._name)
    , _description(v._description)
{
}

Value::Value(const std::string& defaultValue, const bool& require, const std::string& name, const std::string& description)
    : isRequired(require)
    , isSet(false)
    , str(defaultValue)
    , _name(name)
    , _description(description)
{
}

Value::Value(const ChooseList& chooseList, const bool& require, const std::string& name, const std::string& description)
    : Value(chooseList.size() ? *(chooseList.begin()) : "", require, name, description)
{
    for(auto choose : chooseList)
        _chooseList.push_back(choose);
}

// Flag

const Flag Flag::WrongFlag = Flag();

Flag::Flag(const Flag& f)
    : isSet(f.isSet)
    , isDefined(f.isDefined)
    , hasValue(f.hasValue)
    , value(f.value)
    , _longFlag(f._longFlag)
    , _shortFlag(f._shortFlag)
    , _description(f._description)
    , _callBackFunc(f._callBackFunc)
{
}

Flag::Flag(const std::string& lFlag, const std::string& sFlag, const std::string& dscrptn)
    : isSet(false)
    , isDefined(!lFlag.empty() || !sFlag.empty())
    , hasValue(false)
    , value(Value())
    , _longFlag(lFlag)
    , _shortFlag(sFlag)
    , _description(dscrptn)
    , _callBackFunc(nullptr)
{
}

Flag::Flag(const std::string& lFlag, const std::string& sFlag, const std::string& dscrptn, const Value definedValue)
    : Flag(lFlag, sFlag, dscrptn)
{
    hasValue = true;
    value = definedValue;
}

const bool Flag::isValid() const
{
    if (_longFlag.empty() && _shortFlag.empty())
        return false;

    const bool longFlagValid = (_longFlag.size() > 2) && (_longFlag[0] == '-') && (_longFlag[1] == '-');
    const bool shortFlagValid = (_shortFlag.size() == 2) && (_shortFlag[0] == '-');

    if (longFlagValid || shortFlagValid)
        return true;

    return false;
}

// Arg

const Arg Arg::WrongArg = Arg();

Arg::Arg(const Arg& a)
    : Value((Value)a)
    , isDefined(a.isDefined)
{
}

Arg::Arg(const std::string& name, const std::string& description)
    : Value(Value("", !name.empty(), name, description))
    , isDefined(!name.empty())
{
}

Arg::Arg(const std::string& name, const std::string& description, const bool require, const Value& defaultValue)
    : Value(defaultValue)
    , isDefined(!name.empty())
{
    isRequired = require;
    _name = name;
    _description = description;
}

Arg::Arg(const Value& value)
    : Arg("", "", false, value)
{
}

} // namespace argparse
