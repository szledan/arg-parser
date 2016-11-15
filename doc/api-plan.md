Plan of ArgParse 1.0
===

### Construction rules

#### ArgParse
    ArgParse(<interlacedOptions> = "");
    ArgParse( { <option1=value>, <option2=value>, ... } = { "" } );

#### Value
    Value(<default> = "", <required>  = !Value::Required, <name> = "", <description> = "");
    Value( { <choose1 (is default)>, <choose2>, ... }, <required>  = !Value::Required, <name> = "", <description> = "");

#### Flag
    Flag(<long-flag> = "", <short-flag> = "", <description> = "");
    Flag(<long-flag> = "", <short-flag> = "", <description> = "", <default> = Value());

#### Arg
    Arg(<name> = "", <description> = "", <required>  = !Value::Required, <default> = Value());
    Arg(<default> = Value());

### Defination rules
    -s[t]                       def(Flag("[--short]", "-s"[, "description"])); [def(Flag("[--tiny]", "-t"[, "description"])); ...]
    -[s]w <value>               def(Flag("[--short-with]", "-w", "[description]", Value("[default-value]", Value::Required[, ...]))); [def(Flag("[--short]", "-s"[, "description"]));]
    -[s]o [<value>]             def(Flag("[--short-optional]", "-o", "[description]", Value("[default-value]", !Value::Required[, ...]))); [def(Flag("[--short]", "-s"[, "description"]));]
    --long                      def(Flag("--long"[, "-l"[, "description"]]));
    --long-with <value>         def(Flag("--long-with", "[-l]", "[description]", Value("[default-value]", Value::Required[, ...])));
    --long-optional [<value>]   def(Flag("--long-optional", "[-l]", "[description]", Value("[default-value]", !Value::Required[, ...])));
    <argument>                  def(Arg("argument", "[description]", Arg::Required)[, Value([])]);
    [<argument>]                def(Arg("argument", "[description]", !Arg::Required)[, Value([])]);

### Behavior rules

#### Value
    Value = { isRequired=false; isSet=false; str=""; _chooseList={}; _name=""; _desciprtion=""; };

    Value()
      = { isRequired=false; isSet=false; str=""; _chooseList={}; _name=""; _desciprtion=""; };
    Value("default" -> (str), required -> (isRequired), "name" -> (_name), "desciprtion" -> (_description))
      = { isRequired=required; isSet=false; str="default"; _chooseList={}; _name="name"; _desciprtion="desciprtion"; };

    Value( { "default", "A" } -> (_chooseList, str) )
      = { isRequired=false; isSet=false; str="default"; _chooseList={ "default", "A" }; _name=""; _desciprtion=""; };
    Value( { "default", "A" } -> (_chooseList, str), required -> (isRequired), "name" -> (_name), "desciprtion" -> (_description))
      = { isRequired=required; isSet=false; str="default"; _chooseList={ "default", "A" }; _name="name"; _desciprtion="desciprtion"; };

#### Flag
    Flag = { isSet=false; defined=false; hasValue=false; value=Value(); _longFlag=""; _shortFlag=""; _desciprtion=""; _callBackFunc=nullptr; };

    Flag()
      = { isSet=false; defined=false; hasValue=false; value=Value(); _longFlag=""; _shortFlag=""; _desciprtion=""; _callBackFunc=nullptr; };
    Flag("--long" -> (_longFlag,defined), "-s" -> (_shortFlag,defined), "desciprtion" -> (_description))
      = { isSet=false; defined=!_longFlag.empty() || !_shortFlag.empty(); hasValue=false; value=Value(); _longFlag="--long"; _shortFlag="-s"; _desciprtion="desciprtion"; _callBackFunc=nullptr; };
    Flag("--long" -> (_longFlag,defined), "-s" -> (_shortFlag,defined), "desciprtion" -> (_description), value -> (hasValue,value))
      = { isSet=false; defined=!_longFlag.empty() || !_shortFlag.empty(); hasValue=true; value=value; _longFlag="--long"; _shortFlag="-s"; _desciprtion="desciprtion"; _callBackFunc=nullptr; };

#### Arg
    Arg : Value = { { Value }; defined=false; };

    Arg()
      = { Value(); defined=false; };
    Arg("name" -> (_name,defined), "desciprtion" -> (_description), required -> (isRequired), value -> (str|str,_chooseList))
      = { isRequired=required; isSet=false; str=value.str; _chooseList={}|value._chooseList; _name="name"; _desciprtion="desciprtion"; defined=!name.empty(); };
    Arg(value -> (str|str,_chooseList))
      = { str=value.str; isRequired=false; _name=""; _desciprtion=""; _chooseList={}|value._chooseList; isSet=false; defined=false; };

### ArgParse API

    def()
