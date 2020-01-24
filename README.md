# C++ simple implementation of RESTART_CASE like functionality from CommonLisp

Common Lisp exception handling is more advanced than in C++,
it allows not only catch and handle exceptions in top-level code,
but to take some decisions at lower-level and to continue work of
lower-level code from the point of exception.

Imagine situation when at low level a json parser stucks at incorrect number
(there is a letter between digits, for instance).
It cannot decide, important this number for high-level code or not. If it will throw an
exception, whole parsing process will be cancelled.
But with restart-case functionality, high-level code might decide to ignore error, replace
wrong-parsed number by default value and continue parsing.

Of couse, there are a lot of ways to implement such functionality in C++, but this one
raher simple and inspired by the most advanced error-handling system among programming
languages (Common List restart-case).

# TODO

  * Implement subtyping, i.e. when function can specify signature of handler it accepts, without
    using templates:
    
        struct case1{};
        struct case2{};
        
        using restart_case_sig1 = restart_case_signature<
            case1, std::function<int(double, char*)>,
            case2, std::function<double(short, int>
        >;
        
        // default restart_case_signature just rises exceptions
        
        int my_func(data* some_data, const restart_case_sig& rc = restart_case_sig{})
        {
        ...
        }
    
  * Implement an algebra over restart_cases:
  
    rc3 = rc1 + rc2
    rc2 = rc1 - std::tuple<case1, case2>
    rc2 = rc1 * std::tuple<case1, case2>
    
    it is needed for specialisation of handlers through call stack, when mor abstract top-level
    handlers are extended and passed down to lowe-level functions.
