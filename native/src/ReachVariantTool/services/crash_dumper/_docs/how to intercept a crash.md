
# How to intercept a crash

On Windows, you get two opportunities to intercept a crash within your own program. Broadly speaking, you should use these opportunities to [save a minidump](./how%20to%20save%20a%20minidump.md) and then allow your process to die normally.

## Types of crash handlers

### Unhandled exception filters

In C++, the term "exception" refers to an object that that you `throw` or `catch`. Typically these will derive from `std::exception`, though they're not required to; you can `throw` any value; you can even `throw` no value at all.

On Windows, all exceptions that happen "beneath" C++ are implemented as <dfn>structured exceptions</dfn>, and the systems around handling them are known as <dfn>[structured exception handling](https://learn.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp?view=msvc-170)</dfn>. This includes OS-level and hardware-level exceptions, such as access violations and integer divisions by zero. If you compile with MSVC, you can write code that interfaces with this system directly by using the proprietary `__try`, `__except`, and `__finally` keywords.

The [`SetUnhandledExceptionFilter`](https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-setunhandledexceptionfilter) API allows you to register a function that gets called if any structured exception goes uncaught. This function receives information about the exception, including the exact CPU state (registers, etc.) at the time the exception occurred.

For dumping a crash, your unhandled exception filter should use the received exception information to save a minidump, and then return `EXCEPTION_EXECUTE_HANDLER` to invoke the default failure behavior (generally, process termination).

### C++ terminate handlers

The `std::terminate` function in the STL is called automatically by the C++ runtime (e.g. the MSVCRT) under a variety of circumstances, including but not limited to uncaught C++ exceptions. That function, in turn, can be made to call a user-defined handler of type `[[noreturn]] void(*)()` (typedeffed as `std::terminate_handler`) by passing said handler to `std::set_terminate`. The default terminate handler will typically just call `std::abort`, to kill the containing process.

So this is pretty simple, right? Just use `std::set_terminate` to register a handler which gathers error information, saves a minidump, and then calls `std::abort`. Yeah, it *is* pretty simple, if you're not using Microsoft's C++ runtime.

Microsoft completely botched the implementation of `std::terminate`. There's supposed to be one global terminate handler. However, Microsoft's runtime instead stores a terminate handler for each individual thread; therefore every thread in your process must call `std::set_terminate` with your desired handler.

#### What to *do* in your terminate handler

Inside the terminate handler, you can call `std::current_exception` to check if termination is occurring due to an exception that was thrown but (probably)[^exception-misplaced-blame] not caught. If there is an exception, you can query information about the pointer by just re-throwing it with a catch block:

```c++
auto e_ptr = std::current_exception();
if (e_ptr) {
   try {
      std::rethrow_exception(e_ptr);
   } catch (const std::exception& ex) {
      // termination was due to an uncaught exception
   } catch (...) {
      // termination was due to an uncaught exception, but 
      // we don't actually know what the hell we threw
   }
} else {
   // termination was not due to an uncaught exception
}
```

[^exception-misplaced-blame]: Note that if `std::terminate` is called while inside a `catch` block, `std::current_exception` will return a pointer to the caught exception, which you'll end up mistaking for an uncaught exception. There's not *really* anything you can do about this, so I recommend just keeping `catch`es as brief as possible to reduce your chances of doing anything that'd make your program auto-terminate.

Once you've gathered your error information and saved your minidump, you should call `std::abort`. If you're targeting Windows and you're doing your own user-facing messaging (e.g. with `MessageBoxW`), then I recommend calling [`_set_abort_behavior(0, _WRITE_ABORT_MSG)`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/set-abort-behavior?view=msvc-170) first, to disable the default dialog box that shows when `std::abort` is called.

## General rules for crash handlers

Don't heap-allocate memory. You have no way of knowing whether the process heap is corrupted. Stick to stack-allocated variables (including `char` arrays instead of `std::string`), and variables with static storage duration (e.g. fields on a Meyers singleton).

Avoid touching any GUI stuff, be that bare Win32 or a library like Qt. If you inadvertently do something that sends or blocks on a [window message](https://learn.microsoft.com/en-us/windows/win32/learnwin32/window-messages), you can't be sure that the recipient window is properly pumping messages. You can still handle basic user interactions, including telling the user about the error, by using things like [`MessageBoxW`](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messageboxw) with no owning window handle.