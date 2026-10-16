
# How to save a minidump

To save a minidump, you need to call `MiniDumpWriteDump` with the appropriate arguments. If you're doing this from an unhandled exception filter, you can pass in `EXCEPTION_POINTERS` that describe the structured exception; otherwise, just pass `nullptr`.

The real challenge is deciding *when* and *from where* to call `MiniDumpWriteDump`.

## When and where to dump

### In-process, same thread

The easiest and simplest way to take a minidump is to just call that function directly from your unhandled exception filter or `std::terminate_handler`. If you have `EXCEPTION_POINTERS`, then pass those via `MINIDUMP_EXCEPTION_INFORMATION`, making sure to set `ClientPointers` on the latter to `TRUE`.

There are, however, a few problems with this approach:

* If the current thread (i.e. the crashing thread) is close to exhausting its available stack space, then you risk a stack overflow. Since heap allocations aren't safe at this point (your crash may be due to heap corruption), you'll want to be doing things like using a `wchar_t(&)[MAX_PATH]` to hold the file path to save the dump to, so you'll be burning quite a bit of stack space.

* When `MiniDumpWriteDump` is called, it attempts to suspend all threads in the process being dumped. (If it's being called from inside that process, it won't suspend the thread it's running on.) However, `MiniDumpWriteDump` also allocates from the OS heap. Thus, if one of those other threads was in the middle of allocating or freeing memory, that thread will have the heap lock, and `MiniDumpWriteDump` will deadlock and fail.

### In-process, sentinel thread

You can avoid the issues with running out of stack space by spawning a sentinel thread when your process first starts, and have that thread wait for information. When any other thread crashes, the crashing thread can convey information about the crash to the sentinel thread, and wake the sentinel thread. The sentinel thread can then be the one to call `MiniDumpWriteDump`: every thread gets its own stack, so you should have plenty of stack space to work with.

This does not, however, solve the issue of `MiniDumpWriteDump` being vulnerable to a deadlock.

### Out-of-process

The most reliable way to call `MiniDumpWriteDump` is from a separate process; that will completely avoid the risk of `MiniDumpWriteDump` deadlocking if the main process has a thread that is in the middle of allocating or freeing heap memory. (This also means that you can allocate heap memory while handling the crash; so, for example, you can use a `std::wstring` and `std::format` to assemble the desired file path, and to assemble any error messages you might want to display via `MessageBoxW`.) You will of course need interprocess communication, to send error information from the main process to the monitor process.

A common approach described online is to have the monitor process be the parent process, spawning the program that the user interacts with as a child process. DovahKit instead has the monitor process exist as the child process.

## Some other nuances

### Multi-threading

"Crashes," as such, are localized to the thread they occur on. Other threads will continue to run while you're busy handling a crash (i.e. while your unhandled exception filter or `std::terminate_handler` is running). These threads can alter program state. Potentially, you could even have multiple threads crash.

There are two things that I recommend doing in order to account for this:

* You should have a `std::mutex` with static storage duration somewhere (e.g. in a Meyers singleton). Your crash handlers should lock that mutex and never release it. That way, if you experience crashes on multiple threads, only the first crashing thread gets to report a crash; the other crashing threads will deadlock. Deadlocking them prevents them from running to completion (i.e. killing the program before the first crashing thread finishes handling its crash).

* *After* you lock that mutex, you should enumerate all other threads in your process and suspend them.[^suspend-other-threads] This limits the amount of time those threads have to alter any shared program state past the point where the crash happened.

  * If you're in a `std::terminate_handler` and you plan on checking for a current exception, call `std::current_exception` *before* you suspend other threads. Some runtimes (including the MSVCRT) spawn exceptions on the stack and transfer them to the heap when you ask for an `std::exception_ptr`, i.e. they allocate; thus if you suspend other threads, then you risk `std::current_exception` deadlocking on the allocation (the same way `MiniDumpWriteDump` can deadlock for in-process dumps).

Yes, `MiniDumpWriteDump` suspends threads for you. However, you need to create files for it to write to before you can call it, and you may also want to do things like notifying the user that a crash has been detected. Best to make sure no other threads can cause any shenanigans while you're doing that stuff.

[^suspend-other-threads]: The officially recommended way to suspend all other threads is by using the old Toolhelp32 API to [enumerate all threads](https://learn.microsoft.com/en-us/windows/win32/toolhelp/traversing-the-thread-list), and suspend threads in your process. However, note my wording: this API doesn't "enumerate all threads in your process." It "enumerates ***all threads,***" in *every* user-mode process, and (if you don't specifically check the process ID on each thread) Windows *will* let you suspend basically any thread in any other user-mode process. It'll let you suspend *every* thread in *every* other user-mode process and hardlock the entire system, to the point that you have to yank the power cord. Ask me how I know.
  
  Anyway, another problem with that API is that it works by taking a snapshot of all threads on the system; so, it's allocating a huge array, and nearly everything in that array is irrelevant to what we're doing. A more efficient approach is to use `NtGetNextThread`, an undocumented [but publicly known](https://ntdoc.m417z.com/ntgetnextthread) NT API function that lets you directly walk Windows's linked list of threads in your process specifically; you can even filter by access rights (e.g. only look at threads you have permission to suspend). This function takes a process handle, a thread handle to continue iteration from (like how you deal with keys when calling Lua's `next` function to walk a table), and a few other assorted doodads.