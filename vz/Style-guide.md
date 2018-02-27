Coding Style Guidelines for lmi
===============================

Syntax Rules
============

Files
-----

This section covers organization of the source code in files.

### Don't use subdirectories

Use flat file organization, with all source files located in the same root
directory.

### Use lower case file names with underscore separators between words

All files names must use lower case ASCII letters only, with underscores to
separate multiple words if necessary.

### Use `.?pp` extensions

Use `.cpp` extension for the source file, `.hpp` extension for the headers and
`.?pp` for the other kinds of source files, e.g. currently `.tpp` is used for
template definitions which don't need to be included by all the class clients
and `.xpp` is used for the [X-Macros](https://en.wikipedia.org/wiki/X_Macro).

### Use Unix end of line characters

All text files should use Unix EOL format, i.e. `\n` (ASCII LF).

### Use maximal line length of 80

Wrap lines longer than 80 characters.

Exception: Do not wrap copyright lines, containing long list of years.


Indentation and Spaces
----------------------

### Use spaces, not TABs

The indentation unit is 4 spaces, hard TABs must only be used in the makefiles.

### Use half indent for access specifiers

Use 2 spaces indent for `public:`, `protected:` and `private:`.

### Do not insert spaces after keywords or inside parentheses

Write `if(condition)`, without any extra spaces.

### Do insert a space after comma

Write `call_function(arg1, arg2, arg3)`, with a single space after each comma.

Exception: Do not insert space in template instantiations such as
`std::pair<T,U>` or specializations to keep the type a single syntactic unit.

### Put separators on a new line in multi-line statements

When a line needs to be wrapped, indent and vertically align the separators.

For the wrapped function calls, indent the opening parentheses and align it
with the subsequent commas, e.g.:
```
call_function
    (arg1
    ,arg2
    ,arg3
    );
```

For the other expressions, indent and align the separator between the terms:
```
std::cout
    << "Hello "
    << "world"
    << "!"
    << "\n"
    ;
```

### Indent the braces of compound statements

The opening brace of compound statements (`if`, `while`, `switch`, ...) must be
on its own line and indented relatively to the statement line itself. The
closing brace must be vertically aligned with the opening brace.

Notice that this means that the compound statement body has the same
indentation level as the braces surrounding it.

Example:

```
if(condition)
    {
    do_something();
    do_something_else();
    }
```

### Use braces even for compound statement containing a single line

Always use braces for compound statements, even in the situations where the
language rules allow omitting them, e.g.
```
for(;;)
    {
    wait();
    }
```


Naming Conventions
------------------

### Use `snake_case_convention` for identifiers

Classes, enums, functions and variables use the same naming convention.

Notice that many existing classes use legacy `CamelCase` naming convention
instead, they should not be renamed, but new classes must not use camel case.

### Use trailing underscore for member variables

Member variables are distinguished from local variables and function parameters
by the presence of `_` at the end.

### Don't use `a_` prefix for function arguments

Although some legacy code does use this prefix, it shouldn't be used in new
code if possible.


Semantic Rules
==============

Miscellaneous
-------------

### Initialize all non-static data members at the point of declaration

Use braced-initializer-lists in the class definition, e.g.:

```cpp
  int         a {3};
  double      b {1.7321};
  std::string c {"explicit default value"};
  std::string d {}; // when an empty string is wanted
```

Rationale: This makes it impossible to use an uninitialized field and avoids
duplication of the same values among different constructors.


Special Member Functions
------------------------

### Use `= default` instead of `{}` to define special member functions

Avoid writing default constructors, copy constructors and assignment operators
or destructors with an empty body, just replace the body with `= default`
instead.

Rationale: This makes the code clearer, and lets the compiler optimize
better.

### `= delete` copy constructor and assignment operator in non-copyable classes

If a class can't be copied, e.g. because it contains reference members or its
destructor has side effects or even if just because it doesn't make sense to
copy objects of this type, explicitly declare the copy constructor and
assignment operator as deleted.

If it does make sense to return objects from the class from functions, define
move constructor (and, usually, move assignment operator too for symmetry) for
it to allow doing this.

### Define special member functions inline if possible

Define the special member functions inline, directly in the class declaration
whenever possible.

One common case when it is not possible to do it is for the class destructor
in one of the two following situations:

 * The destructor is pure virtual and cannot be defined at the point of
   declaration.
 * The class has smart-pointer members of forward-declared types.

Define the destructor (still using `= default`, if appropriate) in the source
file, out of line, in this case and, in the latter case, use the standard
comment explaining why is it done so:

```cpp
/// Destructor.
///
/// Although it is explicitly defaulted, this destructor cannot be
/// implemented inside the class definition, where a class type that
/// it depends upon is incomplete.
```


Comparisons
-----------

### Write comparison operations in number line order

I.e. always use `<` or `<=` instead of `>` or `>=`.
