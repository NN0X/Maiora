# Maiora Specification v0.1

## Types

### none

A type that represents the absence of a value. Used only in function signatures and function calls.

```maiora
private none foo(none)
{
    IO::print(ascii"This function takes no arguments and returns no value.");
}

private none print(ascii message)
{
    IO::print(message);
}

entry sint64 main(none)
{
    foo(none);
    print(ascii"Hello, Maiora!");
    print(none);

    return 0s;
}
```

Above example demonstrates the use of `none` type in function signatures and calls.
In case of passing `none` as an argument to a function that expects arguments, the function will execute all logic that does not depend on the arguments. This behavior can be prevented by using `strong` keyword or by overloading with `none` as argument.

```maiora
private none foo1(ascii message)
{
    IO::print(message);
}

private none foo1(none)
{
    IO::print(ascii"Overloaded function with none argument.");
}

private strong none foo2(ascii message)
{
    IO::print(message);
}

entry int64 main(none)
{
    foo1("Hello, Maiora!"); // Calls the first overload
    foo1(none);             // Calls the second overload
    foo2(none); // Error: strong function cannot be called with none

    return 0s;
}
```

### uint8, uint16, uint32, uint64

Unsigned integer types of various sizes. Used for representing non-negative integers.

```maiora
private uint64 printUint(uint64 value)
{
    IO::print(ascii"Value: {value}");

    return 0u;
}
```

### sint8, sint16, sint32, sint64

Signed integer types of various sizes. Used for representing both positive and negative integers.

```maiora
private sint64 printInt(sint64 value)
{
    IO::print(ascii"Value: {value}");

    return 0s;
}
```

### float16, float32, float64

Floating-point types of various sizes. Used for representing real numbers with fractional parts.

```maiora
private float64 printFloat(float64 value)
{
    IO::print(ascii"Value: {value}");

    return 0.0f;
}
```

### bool

Boolean type representing true or false values.

```maiora
private bool isEven(sint64 value)
{
    return value % 2s == 0s;
}

private none printBool(bool value)
{
    IO::print(ascii"Value is {value}");
}

entry sint64 main(none)
{
    printBool(isEven(4s)); // Output: Value is true
    printBool(isEven(5s)); // Output: Value is false

    return 0s;
}

```

### ascii

ASCII string type representing a sequence of characters. Used for text manipulation and display.
Uses ASCII encoding for characters. ascii type before string is used to specify formatting.

```maiora
private none printAscii(ascii message)
{
    IO::print(message);
}

private ascii sintToAscii(sint64 value)
{
    return ascii"{value}";
}

entry sint64 main(none)
{
    printAscii(ascii"Hello, Maiora!"); // Output: Hello, Maiora!
    printAscii(sintToAscii(42s)); // Output: 42

    return 0s;
}

```

## Keywords

### entry

The entry keyword is used to define the main entry point of the program. It specifies the function that will be executed when the program starts. The entry function can return any type that is convertible to sint64, which is typically used to indicate the exit status of the program.
There can be only one entry function per program.
Entry function can take any arguments.


Example with zero arguments:
```maiora
entry sint64 main(none)
{
    IO::print(ascii"Hello, Maiora!");
    return 0s;
}
```
Example with arguments:
```maiora
entry sint64 plentifulMain(ascii name, uint16 age, bool isStudent)
{
    IO::print(ascii"Name: {name}, Age: {age}, Is Student: {isStudent}");
    return 0s;
}
```

### private

The private keyword is used to define a function or variable that is only accessible within the current file or function. Function declared in `.ora` file cannot be private. All variables are private by default.

Example of a private function:
```maiora
private none printMessage(ascii message)
{
    IO::print(message);
}
```
Example of a private variable:
```maiora
private none foo(none)
{
    private ascii message = ascii"Hello, Maiora!";
}

entry sint64 main(none)
{
    foo(none);
    IO::print(message); // Error: message is private and cannot be accessed here
    return 0s;
}
```

### public

The public keyword is used to define a function or variable that is accessible from other files or functions. Functions implemented with `public` keyword do not have to be declared in `.ora` file.

Example of a public function:
```maiora
public none printPublicMessage(ascii message)
{
    IO::print(message);
}
```
Example of a public variable:
```maiora
private none foo(none)
{
    public ascii publicMessage = ascii"Hello, Maiora!";
}

entry sint64 main(none)
{
    foo(none);
    IO::print(publicMessage); // Accessing public variable
    return 0s;
}
```
IMPORTANT: Public variables are available from the moment their associated function is called. Their scope is defined by entry function. It is recommended that public variables are only used in entry function.

IMPORTANT: If public variable is declared in a non-entry function they are overwritten every time the function is called. This means that if you call the function multiple times the variable will be instantiated multiple times, which limits performance as it requires getting rid of the previous instance of the variable.

```maiora
private none setPublicMessage(ascii message)
{
    public ascii publicMessage = message; // This will be overwritten every time the function is called
}

entry sint64 main(none)
{
    setPublicMessage(ascii"Hello, Maiora!"); // First call
    IO::print(publicMessage); // Output: Hello, Maiora!

    setPublicMessage(ascii"Goodbye, Maiora!"); // Second call
    IO::print(publicMessage); // Output: Goodbye, Maiora!

    return 0s;
}
```

### strong

The strong keyword is used to define a function that cannot be called with `none` as an argument. This is useful for functions that require specific arguments to operate correctly and should not be called without them.

### declare and implement

The declare keyword is used to declare a function that is implemented in another file. It is used in `.ora` files to declare functions that are implemented in `.mai` files.
Function declared with `declare` keyword must be implemented within file specified as implementation file in the `.ora` file.
Function with `declare` is callable from any file that imports the `.ora` file.

```maiora
// foo.ora
#use Log

declare none log(none); // Declare function that will be implemented in foo.mai
```

```maiora
// foo.mai
#use Log

implement none log(none)
{
    IO::print(ascii"Function foo called.");
}
```

This can be accessed by importing the module used in `foo.ora` and `foo.mai` files.
```maiora
// main.mai
#import Log

entry sint64 main(none)
{
    Log::log(none);
    return 0s;
}

```
or by importing only log function:
```maiora
// main.mai
#import Log.log

entry sint64 main(none)
{
    log(none);
    return 0s;
}
```
