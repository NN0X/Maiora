# Maiora Specification v0.3alpha

## Types

### Variable initialization
All variables must be initialized at the time of declaration. Uninitialized variables are not allowed.

### Type conversion

Implicit casting is disallowed, while explicit casting is:
 - allowed when widening a compatible type (e.g., sint32 to sint64)
 - disallowed when narrowing a compatible type (e.g., sint64 to sint32)
 - disallowed when converting between incompatible types (e.g., sint32 to float32)
 - disallowed when converting between signed and unsigned types (e.g., sint32 to uint32)

All type conversions are allowed when used in an `unsafe` block.

```maiora
sint32 a = 42s;
// sint16 b = sint16(a); will not compile
sint64 b = sint64(a); // allowed

unsafe
{
    sint16 c = sint16(a); // allowed in unsafe block
}
```

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

### float8, float16, float32, float64

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

Same as ascii but allows for UTF-8 characters.

## Keywords

### entry

The entry keyword is used to define the main entry point of the program. It specifies the function that will be executed when the program starts. The entry function can return any type that is convertible to sint64, which is typically used to indicate the exit status of the program.
There can be only one entry function per module.
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

In case of importing modules, the entry functions are invoked at the start of the program in the order of imports.

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

Above code will not compile, because globalVariable is not attached to any function scope.


### enum

```maiora
#import IO

public none foo(sint32 enum Colors)
{
        IO::print(ascii"Color value: {Colors.COLOR_RED}");
}

entry sint32 main(none)
{
        sint32 enum Colors
        {
                COLOR_RED = 0,
                COLOR_GREEN,
                COLOR_BLUE,
                COLOR_BLACK = 100
        }

        foo(Colors);

        return 0s;
}
```

### address and reference
The address keyword is used to indicate variable storing a memory address of a heap-allocated variable.
The reference keyword is used to indicate variable storing a reference to an existing address or variable.

Both follow these rules:
- value under address is mutable, value under reference is immutable
- reference is created from an existing address or variable
- reference has inherently the same scope as the variable it references
- both reference is immutable (cannot be reassigned)
- address can be reallocated with heap keyword but cannot be reassigned to another address
- both address and reference cannot be returned from functions
- address is automatically freed when going out of scope and when reallocated with heap keyword
- reference to an address is pointing to the address variable not to the value under the address
- reading reference to an address implicitly reads the value under the address

Example of using address and reference keywords:
```maiora
#import IO.print

public none foo1(reference sint64 numbers)
{
        print("number at 0: ", numbers[0]);
}

public none foo1(address sint64 numbers)
{
        print("number at 0: ", numbers[0]);
        numbers[1] = 101s;
}

entry sint32 main(none)
{
        address sint64 numbers = heap 10*[0s];
        numbers[0] = 42s;

        reference sint64 numRef = ref numbers;
        foo1(numRef);

        foo1(ref numbers);

        foo1(numbers);

        return 0s;
}
```

### function

The function keyword is used to define a function type. This allows passing functions as arguments to other functions, returning functions from functions, and storing functions in variables.

Example of using function keyword:
```maiora
private none executeFunction(function none f)
{
        IO::print(ascii"Executing function...");
        f(none);
        IO::print(ascii"Function executed.");
}
```

### instance

The instance keyword is used to create an instance of a function. This allows assigning a function instance to a variable. When declaring a variable with the instance keyword the variable will NOT take the return value of the function, but rather the function itself. This means that the variable can be used to use the function as a data object like a C structure or C++ class. Instances by default are allocated on the stack, unless declared with heap keyword. When stack allocated instance is passed to another function, it is implicitly passed by reference. All instances follow the same rules as reference variables.

Example of using instance keyword:
```maiora
private none printMessage(instance message)
{
    IO::print(message);
}

entry sint64 main(none)
{
    private instance printHello = printMessage(ascii"Hello, Maiora!"); // this also calls the function
    private instance printGoodbye = heap printMessage(ascii"Goodbye, Maiora!"); // this also calls the function

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

### Instance keywords

- `get` - retrieves the return value of the instance function
- `call` - calls the instance function with original arguments

```maiora
entry sint64 main(none)
{
    private instance addFunction = add(5s, 10s); // creates an instance of the add function

    private sint64 result = get addFunction; // retrieves the return value of the add function instance

    IO::print(ascii"Result of addition: {result}"); // Output: Result of addition: 15

    call addFunction; // calls the add function instance again

    return 0s;
}

```

### Concurrency

Maiora supports two concurrency models, multithreading and multiprocessing.

#### Multithreading

The `async` keyword is used to define a function that will be executed asynchronously and the variable that will hold the async instance. The `spawn` keyword is used to create a new asynchronous task, and the `join` keyword is used to wait for an asynchronous task to complete.

Example of using async, spawn, and join keywords:
```maiora
private async sint64 asyncAdd(sint64 a, sint64 b)
{
    return a + b;
}

entry sint32 main(none)
{
    private async asyncTask = spawn asyncAdd(5s, 10s);
    private sint64 result = join asyncTask;

    return 0s;
}
```

##### Implicit locking in async functions

All writes to addresses and reads from adresses and references are implicitly locked when used in async functions. This is necessary to provide safe-by-default asynchronous operations. Locking is done with canonical ordering to prevent deadlocks.

NOTE: This will be modified in the future to move from implicit locking to compiler static analysis that would determine if race conditions are possible or not.

Unsafe keyword can be used to disable this behavior.

```maiora
private async none increment(address sint64 counter)
{
    for (uint64 i = 0u; i < 1000u; i = i + 1u)
    {
        counter = counter + 1s;
    }
}

entry sint32 main(none)
{
    address sint64 counter = heap 0s;

    private async asyncTask1 = spawn increment(ref counter);
    private async asyncTask2 = spawn increment(ref counter);

    join asyncTask1;
    join asyncTask2;

    IO::print(ascii"Final counter value: {counter}"); // Output: Final counter value: 2000

    return 0s;
}
```
If async is not joined before going out of scope, the program will automatically join it.

#### Multiprocessing

The `process` keyword is used to define a variable that is a handle to the OS process. The `launch` keyword is used to create a new process with specified function as the entry point, and the joining follows the same rules as threading, with `join` keyword. Processes are isolated from each other and do not share memory space. Inter-process communication mechanisms have to be used to communicate between processes. Those mechanisms are not defined by the language specification and are left to SML.

Functions used with process keyword are required to return sint64 values to indicate process exit status and are limited to arguments that are serializable, so passing addresses, references, instances and functions is not allowed.

Example of using process and launch keywords:
```maiora
private sint64 processAdd(sint64 a, sint64 b)
{
    return a + b;
}

private sint64 someProcessFunction(none)
{
    return 0s;
}

entry sint32 main(none)
{
    private process processTask = launch processAdd(5s, 10s);
    private sint64 result = join processTask;

    private process anotherProcessTask = launch someProcessFunction(none);
    private sint64 anotherResult = join anotherProcessTask;
    if (anotherResult == 0s)
    {
        IO::print(ascii"someProcessFunction completed successfully.");
    }
    else
    {
        IO::print(ascii"someProcessFunction failed with code: {anotherResult}");
    }

    return 0s;
}
```
Note that, while process functions are recommended to only return error codes, those error codes can be used to return any information that fits into sint64 type.

## Key concepts

### Safe by default

Maiora safe by default system is designed to prevent memory safety violations at compile time. If a memory safety violation is detected, the program will not compile.
It is not the responsibility of the programming language to handle program logic violations. These are the responsibility of the programmer.

Below are the key principles necessary to understand Maiora safe by default philosophy.

#### Two-tiered error hierarchy

Maiora follows a two-tiered error hierarchy:
- logic errors (Tier 1)
- hardware errors (Tier 0)

#### Logic errors (Tier 1)

Logic errors are split into two categories:
- memory safety violations (including data-races)
- program logic violations

#### Hardware errors (Tier 0)

Hardware errors include issues such as out-of-memory conditions, hardware faults, and other low-level errors that are outside the control of the programming language. Those errors are considered irrecoverable and have to lead to immediate program termination.

#### Offensive approach

Maiora takes an offensive approach to error handling. This means that the program should be designed to avoid errors altogether, rather than trying to recover from them. If an error occurs, the program should terminate immediately, rather than attempting to continue execution in an undefined state.

#### Practicality of offensive approach

In practice, this means that an ideal Maiora program above certain level of complexity should use a two-tiered structure where there is one supervisor process that is responsible only for monitoring and restarting worker processes that do the actual work. This way, if a worker process encounters a logic error and terminates, the supervisor process can simply restart it without affecting the overall system.
Note that above does not mean that Maiora programs cannot be written as monolithic applications. It simply means that the recommended approach for building robust systems is to use a supervisor-worker architecture and Maiora is designed to facilitate that approach.

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

Instances can also be created with sugar syntax:
```maiora
entry sint64 main(none)
{
    private printMessage printHello = ascii"Hello, Maiora!";
    private printMessage printGoodbye = ascii"Goodbye, Maiora!";

    executeFunctionWithInstance(printHello); // Calls the function instance with "Hello, Maiora!"
    executeFunctionWithInstance(printGoodbye); // Calls the function instance with "Goodbye, Maiora!"

    return 0s;
}
```

#### instance vs function keyword

Why pass functions with instance keyword and not function keyword?
When passing instance as arguments it conveys another information that can be used before calling the function.
So in conclusion, when passing a function only to call it the better way is to use function keyword.
When passing a function to be used as a data object, the better way is to use instance keyword.

#### Stack vs Heap allocation of instances

IMPORTANT: Instances are by default allocated on the stack. To allocate instance on the heap use heap keyword.
```maiora
entry sint64 main(none)
{
    private instance stackInstance = printMessage(ascii"Stack allocated instance");
    private instance heapInstance = heap printMessage(ascii"Heap allocated instance");

    call stackInstance;
    call heapInstance;

    return 0s;
}
```
and similarly with sugar syntax:
```maiora
entry sint64 main(none)
{
    private printMessage stackInstance = ascii"Stack allocated instance";
    private printMessage heapInstance =  heap ascii"Heap allocated instance";

    call stackInstance;
    call heapInstance;

    return 0s;
}
```

#### Composition using instances

Inlining functions within other functions allows for OOP-like composition.

```maiora
private none engine(sint64 power)
{
    public none start(none)
    {
        IO::print(ascii"Engine with power {power} started.");
    }
}

private none car(ascii model, sint64 enginePower)
{
    inline engine(enginePower);

    public none drive(none)
    {
        IO::print(ascii"Driving car model {model}.");
    }
}

entry sint64 main(none)
{
    private instance myCar = car(ascii"SuperCar", 500s);
    myCar.start(none);
    myCar.drive(none);

    return 0s;
}
```

### blocks

#### unsafe

The `unsafe` block is used to encapsulate code that would otherwise be disallowed by the Maiora Safety System. This includes operations such as type casting between incompatible types, pointer arithmetic, and direct memory manipulation.

Example of using `unsafe` block:
```maiora
entry sint64 main(none)
{
    sint64 a = 42s;
    float64 b = 0.0f;

    unsafe
    {
        b = float64(a);
    }

    IO::print(ascii"Value of b: {b}");

    return 0s;
}
```

#### asm

The `asm` block is used to include inline assembly code within Maiora programs. This allows for low-level operations and optimizations that are not directly supported by the Maiora language. `asm` is implicitly unsafe.

Example of using `asm` block:
```maiora
entry sint64 main(none)
{
    sint64 result = 0s;

    asm
    {
        // Example assembly code (syntax may vary based on target architecture)
        mov rax, 5
        mov rbx, 10
        add rax, rbx
        mov result, rax
    }

    IO::print(ascii"Result of assembly addition: {result}");

    return 0s;
}
```

## Standard Module Library

The Standard Module Library (SML) provides a set of pre-defined modules and functions that can be imported and used in Maiora programs. SML modules are maiora source files that have to be provided by the compiler provider. See [SML Specification](sml_specification.md) for more details.

## Error handling

Maiora does not have any language-level error handling mechanisms like exceptions or try-catch blocks. Instead, error handling is left to be implemented by SML providers, according to the SML Specification.
