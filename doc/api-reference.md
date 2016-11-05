ArgParse API Reference
=====
Last modified: **2016-11-05**.
Author(s): Szilard Ledan, @szledan, <szledan@gmail.com>

---

## Public API
Public classes/structures.
```c++
namespace argparse {

class ArgParse;
    enum ErrorCodes;
    struct ArgError;
    struct Counts;
    struct Options;
        struct Option;

struct Arg;
struct Flag;
struct Value;

typedef void (*CallBackFunc)(void);

} // namespace argparse

```
#### The ArgParse class
```c++
class ArgParse {
    // Constructor.
    ArgParse(const OptionList& = {});

    // Add defination of args and flags.
    const Arg& add(const Arg&);
    const Flag& add(const Flag&, CallBackFunc = nullptr);

    // Parse parameter list:
    // * setup value of args, flags;
    // * collect errors if needed;
    // * call back user functions if everything fine.
    const bool parse(const int argc, char* const argv[]);

    // Generate a help string.
    const std::string help();
    // Generate an error string.
    const std::string error();

    // Return the error list.
    const std::vector<ArgError>& errors();

    // Check flag. If user given the flag in 'argv' then returns true.
    const bool checkFlag(const std::string& flagStr);
    // If user given the flag in 'argv' and <T> type has operator>>()
    // then return true, and read/convert string of value to <T>.
    template<typename T>
    const bool checkFlagAndReadValue(const std::string& flagStr, T* value);

    // TODO: missing description.
    Arg const& operator[](const std::size_t& idx);
    // TODO: missing description.
    Arg const& operator[](const int idx);
    // TODO: missing description.
    Flag const& operator[](const std::string& idx);
    // TODO: missing description.
    Flag const& operator[](const char* idx);

    // Public member functions.
    Counts counts;
    Options options;
};
```
#### The Arg structure
```c++
struct Arg : Value {
    static const bool IsNeeded = true;
    // Constructors.
    Arg(const Arg& a);
    // Define an argument with:
    // * 'name' and 'description', this needed to the help generator
    // * set 'isNeeded' if this argument is needed.
    // * set 'defaultValue'.
    Arg(const std::string& name = "",
        const std::string& description = "",
        const bool isNeeded = false,
        const Value& defaultValue = Value());
    // Define an argument with only a value.
    // Note: The 'help()' doesn't generate to the help string
    //       this way defined arguments.
    Arg(const Value& value);

    // Setup argument with value and check true the 'isSet' member.
    void setArg(const std::string& value);

    // This member indicates the argument usage. (Default = false.)
    bool isSet;

    // These members are private only for careful usage.
// private:
    bool _isArgNeeded;
    CallBackFunc _callBackFunc;
};
```
#### The Flag structure and CallBackFunc type
```c++
typedef void (*CallBackFunc)(void);

struct Flag {
    // Constructors.
    Flag(const Flag& f);
    // Define a simple setable flag. (Without value.)
    Flag(const std::string& longFlag = "",
         const std::string& shortFlag = "",
         const std::string& description = "");
    // Define flag with value.
    Flag(const std::string& longFlag,
         const std::string& shortFlag,
         const std::string& description,
         const Value value);

    // This member indicates the flag usage. (Default = false.)
    bool isSet;
    // This member indicates if flag has value. (Default = false.)
    bool hasValue;
    Value value;

    // These members are private only for careful usage.
// private:
    std::string _longFlag;
    std::string _shortFlag;
    std::string _description;
    CallBackFunc _callBackFunc;
};
```
#### The Value structure
```c++
typedef std::initializer_list<std::string> ChooseList;

struct Value {
    // Constructors.
    Value(const Value& v);
    // Define a simple value.
    // Note: The empty 'defaultValue' means this value
    //       needed.
    Value(const std::string& defaultValue = "",
          const std::string& name = "",
          const std::string& description = "");
    // Define value with chooser list.
    // Note: The empty 'defaultValue' means this value
    //       needed.
    Value(const std::string& defaultValue,
          const ChooseList& chooseList,
          const std::string& name = "",
          const std::string& description = "");

    const bool empty() const;

    // This member contans the value as string.
    std::string str;

    // These members are private only for careful usage.
// private:
    const std::string _getChoosesStr(bool full = true) const;

    std::string _name;
    std::string _description;
    std::vector<std::string> _chooseList;
    bool _isValueNeeded;
};
```
#### The ArgParse::ArgError structure and ArgParse::ErrorCodes enumerate.
```c++
enum ErrorCodes {
    NoError = 0,
    ErrorRequiredArgumentMissing,
    ErrorRequiredFlagValueMissing,
    ErrorARGVEmpty,
};

struct ArgError {
    enum ArgErrorType {
        GeneralType,
        ArgType,
        FlagType,
    } const type;

    // Define the error.
    const ErrorCodes errorCode;
    // Connect the problematic element. (Note: '_ptr' is maybe nullptr.)
    union {
        const void* _ptr;
        const Arg* arg;
        const Flag* flag;
    };
    const std::string errorMessage;
};
```
#### The ArgParse::Counts structure
```c++
// Count the (un)defined args and flags in 'argv'.
struct Counts {
    size_t undefinedArgs;
    size_t definedArgs;
    size_t undefinedFlags;
    size_t definedFlags;
} = { 0u, 0u, 0u, 0u };
```
#### The ArgParse::Options and ArgParse::Options::Option structure
```c++
struct Options {
    struct Option;

    Option programName = { "program-name", { "", Option::Value::NotSet } },
    Option tab = { "tab", { "    ", Option::Value::FourSpace } },
    Option mode = { "mode", { "compact", Option::Value::Set } },
    Option helpFlag = { "help-flag", { "on", Option::Value::Set } };

    static void set(Option&, const std::string& value, const int& state = Option::Value::Unused);
};

struct Option {
    struct Value {
        // Setting type of option value.
        enum {
            Unused = -1,
            // Boolean.
            NotSet = 0,
            Set = 1,
            // Not boolean.
            NotBool,
            FourSpace,
        };
        const bool isSet() const { return state != NotSet; }

        std::string value;
        int state;
    };

    // Name for parsing.
    const std::string name;
    // Default value.
    const Value init;
    Value current;
};
```
