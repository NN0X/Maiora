# Maiora Specification v0.1 \[OUTDATED\]

## Types

### none

A type that represents the absence of a value. Used only in function signatures and function calls.

```maiora
private none foo(none)
{
    IO::print(ascii"This function takes no arguments and returns no value.");
}

private none print(ascii[] message, uint64 size)
{
    IO::print(message, size);
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
In case of passing `none` as an argument to a function that expects arguments, the function will execute all logic that does not depend on the arguments. This behavior can be prevented by using `strong` keyword or by overloading with `none` as argument. It's important to note that when statically compiled, `strong` is always implied.

But then why do the functions have the ability to be called with `none` as an argument?
This is because Maiora can be run in an interpreter where existence of a variable (for example in a save file) is not guaranteed. In such cases, the function can be called with `none` to execute the logic that does not depend on the arguments.

In the interpreters that are compatible with Maiora Specification, the default behavior for such cases is to engage emergency handler that will add the function to backlog and execute it later when the required arguments are available.

```maiora
#import Types.string;

private none foo1(instance message)
{
    IO::print(message);
}

private none foo1(none)
{
    IO::print(ascii"Overloaded function with none argument.");
}

private strong none foo2(instance message)
{
    IO::print(message);
}

entry int64 main(none)
{
    foo1(ascii"Hello, Maiora!"); // Calls the first overload
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

ascii is a character type used for representing characters in the ASCII encoding.

```maiora
private none printAscii(ascii[] message, uint64 size)
{
    IO::print(message, size);
}

private ascii sintToAscii(sint64 value)
{
    return ascii'{value}'; // Converts sint64 to ascii representation
}

entry sint64 main(none)
{
    printAscii(ascii"Hello, Maiora!"); // Output: Hello, Maiora!
    printAscii(sintToAscii(42s)); // Output: 42

    return 0s;
}
```

Notice that in above example that sintToAscii returns ascii'{value}' while printAscii is called with ascii"Hello, Maiora!".
When using single quotes, the value is converted to ascii representation.
When using double quotes, the value is converted to two variables: array of ascii characters and a size of the array in uint64.


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
Variables are private by default, but can be explicitly marked as private for clarity. Functions must be explicitly marked as private.

Example of a private function:
```maiora
private none printMessage(instance message)
{
    IO::print(message);
}
```
Example of a private variable:
```maiora
private none foo(none)
{
    private instance message = Types::string("Hello, Maiora!");
}

entry sint64 main(none)
{
    private instance fooInstance = foo(none); // creates an instance of the function foo
    foo(none);
    IO::print(fooInstance.message); // Error: message is private and cannot be accessed here
    return 0s;
}
```

### public

The public keyword is used to define a function or variable that is accessible from other files or functions.
Functions are public by default, but can be explicitly marked as public for clarity. Variables must be explicitly marked as public to be accessible outside of their scope.

Example of a public function:
```maiora
public none printPublicMessage(instance message)
{
    IO::print(message);
}
```
Example of a public variable:
```maiora
private none foo(none)
{
    public instance publicMessage = Types::string(ascii"Hello, Maiora!");
}

entry sint64 main(none)
{
    private instance fooInstance = foo(none); // creates an instance of the function foo
    foo(none);
    IO::print(fooInstance.publicMessage); // Accessing public variable
    return 0s;
}
```
IMPORTANT: Public variables are available from the instances of corresponding functions. Their scope is defined by the scope of the instance variable.

IMPORTANT: There is no such thing as a global variable in Maiora. The most "global" scope is the scope of the entry function.

```maiora

public sint64 globalVariable = 42s;

entry sint64 main(none)
{
    IO::print(ascii"Global variable: {globalVariable}"); // Accessing public variable
    return 0s;
}
```

Above code will not compile, because globalVariable is not attached to any function instance.


### instance

The instance keyword is used to create an instance of a function. This allows assigning a function instance to a variable. When declaring a variable with the instance keyword the variable will NOT take the return value of the function, but rather the function itself. This means that the variable can be used to use the function as a data object like a C structure or C++ class.

Example of using instance keyword:
```maiora
private none printMessage(instance message)
{
    IO::print(message);
}

entry sint64 main(none)
{
    private instance printHello = printMessage(ascii"Hello, Maiora!"); // this also calls the function
    private instance printGoodbye = printMessage(ascii"Goodbye, Maiora!"); // this also calls the function

    return 0s;
}
```

Above example demonstrates how to create instances of functions and call them later, but also shows a problem with this implementation of printMessage function as it probably should not print anything when called with instance keyword. Below example shows the proper way to use instance keyword with functions that are not supposed to be called immediately.

```maiora
private none printMessage(instance pmessage)
{
    private instance message = pmessage;

    public none call(none)
    {
        IO::print(message);
    }
}

entry sint64 main(none)
{
    private instance printHello = printMessage(ascii"Hello, Maiora!"); // creates an instance of the function
    private instance printGoodbye = printMessage(ascii"Goodbye, Maiora!"); // creates an instance of the function

    printHello.call(none); // Calls the function instance
    printGoodbye.call(none); // Calls the function instance

    return 0s;
}
```

As you can see in the above example, the `printMessage` function creates an instance variable `message` that holds the message to be printed. The `call` function is then used to print the message when needed.

You can also see that ascii"Hello, Maiora!" is passed as an instance. But why is that?
The string type defined in Maiora is actually a function `public none string(ascii[] pstr, uint64 size)` that is implemented in the module `Types`.

There is a cleaner way to ineract with Types instances (in actuality, all instances in Maiora) as shown below.
```maiora

private none string(ascii[] pstr, uint64 size)
{
    private instance str = pstr;
    private uint64 strSize = size;

    public none print(none)
    {
        IO::print(str, strSize);
    }
}

entry sint64 main(none)
{
    private string hello = ascii"Hello, Maiora!"; // Creates an instance of the string function
    // line below is equivalent to the line above
    // private instance hello = string(ascii"Hello, Maiora!");
    // REMINDER: ascii"Hello, Maiora!" colapses to ascii[] and uint64 size automatically
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

    private none printArgs(instance arg1, uint16 arg2)
    {
        IO::print(ascii"Arguments: {arg1}, {arg2}");
    }

    executeFunctionWithNoReturn(printHello);
    executeFunctionWithReturn(returnValue);
    executeFunctionWithArgs(printArgs, string(ascii"Hello"), 123u);

    return 0s;
}
```

Passing instances of functions as arguments:
```maiora
private none executeFunctionWithInstance(instance f)
{
    call f; // Calls the function instance
}

private none printMessage(instance message)
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

Why pass functions with instance keyword and not function keyword?
When passing instance as arguments it conveys another information that can be used before calling the function.
So in conclusion, when passing a function only to call it the better way is to use function keyword.
When passing a function to be used as a data object, the better way is to use instance keyword.
