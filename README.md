# C++ simple implementation of RESTART_CASE like functionality from CommonLisp

Common Lisp exception handling is more advanced then in C++,
it allows not only catch and handle exceptions in top-level code,
but to take some desisions at lower-level and to continue work of
lower-level code.

Imagine situation when at low level a json parser works and stucks at incorrect number
(there is a letter between digits, for instance).
It cannot decide, important this number or not for high-level code. If it will throw an
exception, whole parsing process will be cancelled.
But with restart-case functionality, high-level code might decide to ignore error, replace
wrong-parsed number by default value and continue parsing.

Of couse, there are a lot of ways to implement such functionality in C++, but this one
raher simple and inspired by the most advanced error-handling system among programming
languages (Common List restart-case).
