Plan of ArgParse 1.0
===

### Construction rules

#### ArgParse
    ArgParse(<interlacedOptions> = "");
    ArgParse( { <option1=value>, <option2=value>, ... } = { "" } );

#### Value
    Value(<default> = "", <required>  = !Value::Required, <name> = "", <description> = "");
    Value( { <choose1 (is default)>, <choose2>, ... } = { "" }, <required>  = !Value::Required, <name> = "", <description> = "");

#### Arg
    Arg(<name> = "", <description> = "", <required>  = !Value::Required, <default> = Value());
    Arg(<default> = Value());

#### Flag
    Flag(<long-flag> = "", <short-flag> = "", <description> = "");
    Flag(<long-flag> = "", <short-flag> = "", <description> = "", <default> = Value());

### Defination rules
    <argument>                  ArgParser::def(Arg("argument", "[description]", Arg::Required)[, Value([])]);
    [<argument>]                ArgParser::def(Arg("argument", "[description]", !Arg::Required)[, Value([])]);
    -s[t]                       ArgParser::def(Flag("[--short]", "-s"[, "description"])); [ArgParser::def(Flag("[--tiny]", "-t"[, "description"])); ...]
    -[s]w <value>               ArgParser::def(Flag("[--short-with]", "-w", "[description]", Value("[default-value]", Value::Required[, ...]))); [ArgParser::def(Flag("[--short]", "-s"[, "description"]));]
    -[s]o [<value>]             ArgParser::def(Flag("[--short-optional]", "-o", "[description]", Value("[default-value]", !Value::Required[, ...]))); [ArgParser::def(Flag("[--short]", "-s"[, "description"]));]
    --long                      ArgParser::def(Flag("--long"[, "-l"[, "description"]]));
    --long-with <value>         ArgParser::def(Flag("--long-with", "[-l]", "[description]", Value("[default-value]", Value::Required[, ...])));
    --long-optional [<value>]   ArgParser::def(Flag("--long-optional", "[-l]", "[description]", Value("[default-value]", !Value::Required[, ...])));

### Behavior rules

#### Value
    Value = { str=""; isRequired=false; _name=""; _desciprtion="", _chooseList={} };

    Value()
      = { str=""; isRequired=false; _name=""; _desciprtion="", _chooseList={} };
    Value("default" -> (str), required -> (isRequired), "name" -> (_name), "desciprtion" -> (_description))
      = { str="default"; isRequired=required; _name="name"; _desciprtion="desciprtion", _chooseList={} };

    Value( { "default", "A" } -> (_chooseList, str) )
      = { str="default"; isRequired=false; _name=""; _desciprtion="", _chooseList={ "default", "A" } };
    Value( { "default", "A" } -> (_chooseList, str), required -> (isRequired), "name" -> (_name), "desciprtion" -> (_description))
      = { str="default"; isRequired=required; _name="name"; _desciprtion="desciprtion", _chooseList={ "default", "A" } };

#### Arg
    Arg = { str=""; isRequired=false; _name=""; _desciprtion="", _chooseList={}, isSet=false, defined=flase };

    Arg()
      = { str=""; isRequired=false; _name=""; _desciprtion="", _chooseList={}, isSet=false, defined=false };
    Arg("name" -> (_name), "desciprtion" -> (_description), required -> (isRequired), value -> (str|str,_chooseList))
      = { str=value.str; isRequired=required; _name="name"; _desciprtion="desciprtion", _chooseList={}|value._chooseList, isSet=false, defined=!name.empty() };
    Arg(value -> (str|str,_chooseList))
      = { str=value.str; isRequired=false; _name=""; _desciprtion="", _chooseList={}|value._chooseList };

#### Flag
    Flag = { str=""; isRequired=false; _name=""; _desciprtion="", _chooseList={}, isSet=false, defined=flase };

    Flag()
      = { str=""; isRequired=false; _name=""; _desciprtion="", _chooseList={}, isSet=false, defined=false };
    Flag("name" -> (_name), "desciprtion" -> (_description), required -> (isRequired), value -> (str|str,_chooseList))
      = { str=value.str; isRequired=required; _name="name"; _desciprtion="desciprtion", _chooseList={}|value._chooseList, isSet=false, defined=!name.empty() };
    Flag(value -> (str|str,_chooseList))
      = { str=value.str; isRequired=false; _name=""; _desciprtion="", _chooseList={}|value._chooseList };

    // interlacedOptions="program.name=program"
