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


--- running test class declaration ---
new template block
end of template
new braceblock
end of scope
datatype int
grouped pattern 7
end of statement
grouped pattern 18
end of statement
- : start group {}
    - : start group class
        < : start group template
            class : class keyword
            T : raw
        end group

        class : class keyword
        apa : raw
        { : start group {}
            - : start group variable declaration
                int : type
                x : raw
            end group

            ;
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
- : start group type 0
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

--- running test functions ---
new paranthesis block
end of scope
new braceblock
end of scope
datatype int
grouped pattern 7
datatype int
grouped pattern 7
grouped pattern 8
end of statement
- : start group type 0
    - : start group function declaration
        - : start group variable declaration
            int : type
            main : raw
        end group

        ( : start group ()
            - : start group variable declaration
                int : type
                x : raw
            end group

        end group

    end group

    { : start group {}
        return : return keyword
        x : raw
        ;
    end group

end group

 --> success 

--- running test multiple character operator ---
 --> success 

--- running test multiple word datatypes ---
grouped pattern 10
- : start group {}
    - : start group for
        for : control statement keyword
        ( : start group ()
            - : start group assignment
                - : start group variable declaration
                    int : type
                    i : raw
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
                - : start group variable declaration
                    int : type
                    x : raw
                end group

                = : equals
            end group

            i : raw
            ;
        end group

    end group

end group

 --> success 

--- running test paranthesis ---
new paranthesis block
end of scope
- : start group type 0
    ( : start group ()
        int : raw
        x : raw
    end group

end group

 --> success 

--- running test second pass ---
new paranthesis block
end of scope
new braceblock
end of scope
datatype int
grouped pattern 7
datatype int
grouped pattern 7
end of statement
datatype char
grouped pattern 7
grouped pattern 8
datatype int
grouped pattern 7
end of statement
end of statement
end of statement
- : start group type 0
    - : start group function declaration
        - : start group variable declaration
            int : type
            main : raw
        end group

        ( : start group ()
            - : start group variable declaration
                int : type
                argc : raw
            end group

            ,
            - : start group variable declaration
                char : type
                argv : raw
            end group

        end group

    end group

    { : start group {}
        - : start group variable declaration
            int : type
            x : raw
        end group

        ;
        cout : raw
        << : operator
        "hello world" : raw
        << : operator
        endl : raw
        ;
        return : return keyword
        0
        ;
    end group

end group

 --> success 

--- running test templates ---
new template block
end of template
new paranthesis block
end of scope
datatype int
end of statement
- : start group type 0
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


