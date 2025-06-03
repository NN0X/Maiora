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

### utf

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

The private keyword is used to define a function or variable that is only accessible within the current file or function.

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
IMPORTANT: Public variables are available from the moment their associated function is called. Their scope is defined by the entry function. It is recommended that public variables are only used in entry function.

IMPORTANT: If public variable is declared in a non-entry function they are overwritten every time the function is called. This means that if you call the function multiple times the variable will be instantiated multiple times.

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

### pack

The pack keyword is used to define a function that can is a member of a module.

For example IO module is defined as follows:
```maiora
#module IO

#type string = ascii|utf;

pack none print(string message)
{
    // Implementation of print function
}

pack none printerr(string message)
{
    // Implementation of printerr function
}

pack bool openFile(string path)
{
    // Implementation of openFile function

    return true;
}

pack entry sint64 initIO(none)
{
    // Initialization code for IO module
    return 0s;
}

```

### instance

The instance keyword is used to create an instance of a function. This allows assigning a function instance to a variable. When declaring a variable with the instance keyword the variable will NOT take the return value of the function, but rather the function itself. This means that the variable can be used to call the function later.

Example of using instance keyword:
```maiora
private none printMessage(ascii message)
{
    IO::print(message);
}

entry sint64 main(none)
{
    private instance printHello = printMessage(ascii"Hello, Maiora!"); // this also calls the function
    private instance printGoodbye = printMessage(ascii"Goodbye, Maiora!"); // this also calls the function

    call printHello; // Calls the function with "Hello, Maiora!"
    call printGoodbye; // Calls the function with "Goodbye, Maiora!"

    return 0s;
}
```

### strong

The strong keyword is used to define a function that cannot be called with `none` as an argument. This is useful for functions that require specific arguments to operate correctly and should not be called without them.

## Key concepts

### Functions as data objects

In contrast to C/C++ and similar languages, Maiora does not have a concept of a structure or class. Instead, it uses functions as data objects. This means that functions can be passed as arguments to other functions, returned from functions, and stored in variables.

Example of a function that takes on a job similar to a class or structure:
```maiora
private none initPerson(ascii name, uint16 age)
{
    private ascii personName = name;
    private uint16 personAge = age;

    public none printPerson(none)
    {
        IO::print(ascii"Name: {personName}, Age: {personAge}");
    }
}

entry sint64 main(none)
{
    private instance personInstance1 = initPerson(ascii"John Doe", 30u);
    private instance personInstance2 = initPerson(ascii"Jane Smith", 25u);

    personInstance1.printPerson(none); // Output: Name: John Doe, Age: 30
    personInstance2.printPerson(none); // Output: Name: Jane Smith, Age: 25

    return 0s;
}
```

Example of a function that takes another function as an argument:
```maiora
private none executeFunctionWithNoReturn(function none f)
{
    IO::print(ascii"Executing function...");
    f(none);
    IO::print(ascii"Function executed.");
}

private none executeFunctionWithReturn(function sint64 f)
{
    IO::print(ascii"Executing function...");
    private sint64 var = f(none);
    IO::print(ascii"Function executed.");
    IO::print(ascii"Return value: {var}");
}

private none executeFunctionWithArgs(function none f, ascii arg1, uint16 arg2)
{
    IO::print(ascii"Executing function with arguments...");
    f(arg1, arg2);
    IO::print(ascii"Function executed with arguments.");
}

entry sint64 main(none)
{
    private none printHello(none)
    {
        IO::print(ascii"Hello, Maiora!");
    }

    private sint64 returnValue(none)
    {
        return 42s;
    }

    private none printArgs(ascii arg1, uint16 arg2)
    {
        IO::print(ascii"Arguments: {arg1}, {arg2}");
    }

    executeFunctionWithNoReturn(printHello);
    executeFunctionWithReturn(returnValue);
    executeFunctionWithArgs(printArgs, ascii"Hello", 123u);

    return 0s;
}
```

Passing instances of functions as arguments:
```maiora
private none executeFunctionWithInstance(instance f)
{
    call f; // Calls the function instance
}

private none printMessage(ascii message)
{
    IO::print(message);
}

entry sint64 main(none)
{
    private instance printHello = printMessage(ascii"Hello, Maiora!");
    private instance printGoodbye = printMessage(ascii"Goodbye, Maiora!");

    executeFunctionWithInstance(printHello); // Calls the function instance with "Hello, Maiora!"
    executeFunctionWithInstance(printGoodbye); // Calls the function instance with "Goodbye, Maiora!"

    return 0s;
}
```

Output of the above code will be:
```plain
Hello, Maiora!
Goodbye, Maiora!
Hello, Maiora!
Goodbye, Maiora!
```
