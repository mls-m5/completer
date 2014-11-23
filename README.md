What is this
-----------
This project aims to do the almost impossible (according to the internet) task of parsing c++ files, in a simple and relaxed (for the programmer, me) way.

This is done by
1: Tokenize the file
2: Group expressions depending on what type it is
3: Do some analyse on that (not implemented yet)


Goals
-------
The goals with this project is to have at least
* A commandline tool for code completion
* A commandline tool for refactoring (renaming variables, types..)

Secondary goals
* Cool source tree outputs
* Parsable files for third party tools


Progress - Appendix..
-----------

This is part of the current test output

==== Starts test suit tests/sourcetree-test.cpp ====

--- running test assignment ---
datatype int
grouped pattern 6
grouped pattern 14
end of statement
- : type : start group type 0
    - : start group assignment
        - (i) : start group variable definition
            int (i) : type
            i (i) : name
        end group

        = : equals
    end group

    0
    ;
end group

 --> success 

--- running test basic data type ---
datatype int
grouped pattern 6
end of statement
 --> success 

--- running test class declaration ---
new template block
end of template
new braceblock
end of scope
datatype int
grouped pattern 6
end of statement
grouped pattern 21
end of statement
- : type : start group type 0
    - (Apa) : start group class
        < (Apa) : start group template
            class (Apa) : class keyword
            T (Apa) : raw
        end group

        class (Apa) : class keyword
        Apa (Apa) : declaration name
        { (Apa) : start group {}
            - (Apa::x) : start group variable definition
                int (Apa::x) : type
                x (Apa::x) : name
            end group

            ; (Apa)
        end group

    end group

    ;
end group

 --> success 

--- running test create source tree ---
new paranthesis block
end of scope
new braceblock
end of scope
- : type : start group type 0
    int : raw
    main : raw
    ( : start group ()
        int : raw
        argc : raw
        ,
        char : raw
        * : operator
        * : operator
        argv : raw
    end group

    { : start group {}
        cout : raw
        << : operator
        "hello world" : raw
        << : operator
        endl : raw
        ;
        return : raw
        0 : raw
        ;
    end group

end group

 --> success 

--- running test for ---
new paranthesis block
end of scope
new braceblock
end of scope
datatype int
grouped pattern 6
grouped pattern 14
end of statement
end of statement
datatype int
grouped pattern 6
grouped pattern 14
end of statement
grouped pattern 13
- : start group {}
    - : start group for
        for : control statement keyword
        ( : start group ()
            - : start group assignment
                - (i) : start group variable definition
                    int (i) : type
                    i (i) : name
                end group

                = : equals
            end group

            0
            ;
            i : raw
            < : operator
            10
            ;
            ++ : operator
            i : raw
        end group

        { : start group {}
            - : start group assignment
                - (x) : start group variable definition
                    int (x) : type
                    x (x) : name
                end group

                = : equals
            end group

            i : raw
            ;
        end group

    end group

end group

 --> success 

--- running test functions ---
new paranthesis block
end of scope
new braceblock
end of scope
datatype int
grouped pattern 6
datatype int
grouped pattern 6
grouped pattern 9
end of statement
grouped pattern 10
- : type : start group type 0
    - : start group function definition
        - : start group function declaration
            - (main) : start group variable definition
                int (main) : type
                main (main) : name
            end group

            ( : start group ()
                - (x) : start group variable definition
                    int (x) : type
                    x (x) : name
                end group

            end group

        end group

        { : start group {}
            return : return keyword
            x : raw
            ;
        end group

    end group

end group

 --> success 

--- running test lambda functions ---
new bracket block
end of brackets
new paranthesis block
end of scope
new braceblock
end of scope
datatype int
grouped pattern 6
end of statement
grouped pattern 11
- : type : start group type 0
    - : start group lambda function
        [
        ( : start group ()
            - (x) : start group variable definition
                int (x) : type
                x (x) : name
            end group

        end group

        { : start group {}
            cout : raw
            << : operator
            "hej" : raw
            << : operator
            endl : raw
            ;
        end group

    end group

end group

new bracket block
end of brackets
new paranthesis block
end of scope
new braceblock
end of scope
datatype auto
grouped pattern 6
grouped pattern 14
datatype int
grouped pattern 6
end of statement
end of statement
- : type : start group type 0
    - : start group assignment
        - (f) : start group variable definition
            auto (f) : type
            f (f) : name
        end group

        = : equals
    end group

    [
    ( : start group ()
        - (x) : start group variable definition
            int (x) : type
            x (x) : name
        end group

    end group

    { : start group {}
        cout : raw
        << : operator
        "hej" : raw
        << : operator
        endl : raw
        ;
    end group

    ;
end group

 --> success 

--- running test multiple character operator ---
 --> success 

--- running test multiple word datatypes ---
datatype long
multi word data type long int
grouped pattern 6
end of statement
- : type : start group type 0
    - (apa) : start group variable definition
        long int (apa) : type
        apa (apa) : name
    end group

    ;
end group

 --> success 

--- running test paranthesis ---
new paranthesis block
end of scope
- : type : start group type 0
    ( : start group ()
        int : raw
        x : raw
    end group

end group

 --> success 

--- running test preprocessor command ---
skipping preprocessor command
datatype int
grouped pattern 6
end of statement
- : type : start group type 0
    - (apa) : start group variable definition
        int (apa) : type
        apa (apa) : name
    end group

    ;
end group

 --> success 

--- running test second pass ---
new paranthesis block
end of scope
new braceblock
end of scope
datatype int
grouped pattern 6
datatype int
grouped pattern 6
end of statement
datatype char
grouped pattern 6
grouped pattern 9
datatype int
grouped pattern 6
end of statement
end of statement
end of statement
grouped pattern 10
 --> success 

--- running test struct declaration ---
new template block
end of template
new braceblock
end of scope
datatype int
grouped pattern 6
end of statement
grouped pattern 23
end of statement
- : type : start group type 0
    - (apa) : start group struct
        < (apa) : start group template
            class (apa) : class keyword
            T (apa) : raw
        end group

        struct (apa)
        apa (apa) : declaration name
        { (apa) : start group {}
            - (apa::x) : start group variable definition
                int (apa::x) : type
                x (apa::x) : name
            end group

            ; (apa)
        end group

    end group

    ;
end group

 --> success 

--- running test templates ---
new template block
end of template
new paranthesis block
end of scope
datatype int
end of statement
- : type : start group type 0
    < : start group template
        class : class keyword
        T : raw
    end group

    int : type
    apa : raw
    ( : start group ()
        T : raw
        bepa : raw
    end group

    ;
end group

 --> success 

--- running test user defined datatype ---
new braceblock
end of scope
datatype int
grouped pattern 6
grouped pattern 21
end of statement
end of statement
- : type : start group type 0
    - (Apa) : start group class
        class (Apa) : class keyword
        Apa (Apa) : declaration name
        { (Apa) : start group {}
            - (Apa::x) : start group variable definition
                int (Apa::x) : type
                x (Apa::x) : name
            end group

        end group

    end group

    ;
    Apa : raw
    apa : raw
    ;
end group

tests/sourcetree-test.cpp:63: 0: not implemented
 --> failed


MISSLYCKADES
Failed: 1 Succeded: 14

