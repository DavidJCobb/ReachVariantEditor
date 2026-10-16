
# Interprocess communication

As explained in "[How to save a minidump](./how%20to%20save%20a%20minidump.md)," the only reliable way to save a minidump is by having a secondary process save it; saving a minidump from inside the process being dumped can lead to deadlocks. Thus we need a way for our main process, in the event of a crash, to communicate information about that crash to our monitor process, so the latter can then notify the user and save a minidump.

This is actually the first time I've ever implemented a multi-process program with IPC, so this article will go over some of the basics as well as what DovahKit in specific is doing.

## Which process does what?

The most discoverable article about collecting an out-of-process dump is [this 2022 article by Phillip Trudeau-Tavara](https://lance.handmade.network/blog/p/8491-automated_crash_reporting_in_basically_one_400-line_function). His approach is to have the parent process do crash reporting by attaching itself to the child process (which is what the end user actually interacts with) as a debugger. He [waits for debug events](https://learn.microsoft.com/en-us/windows/win32/api/debugapi/nf-debugapi-waitfordebugevent), which include unhandled structured exceptions.

This approach requires that he have his crash-monitoring process cloak itself from the target process, so that `IsDebuggerPresent` returns false. It also means that he has to configure his *actual* debugger to pass a command line parameter (`-no-crash-handler`), which his program has to check for. Plus, it feels strange to me to have the parent process monitor for crashes and the child process actually do work; it feels strange that the process the user directly starts isn't the process they interact with.

I didn't really want to deal with any of that, so in RVT, the parent process is the main process (that the user interacts with), and the child process is the monitor process. The former manually [intercepts crashes](./how%20to%20intercept%20a%20crash.md), transmits the information to the latter via IPC, and waits for an acknowledgment before allowing itself to terminate.

## IPC

The Win32 API offers a construct called "[pipes](https://learn.microsoft.com/en-us/windows/win32/ipc/pipes)" for interprocess communication. When you create a pipe, you're given handles for reading and writing to the pipe. When you create a process, you can dictate that that process use pipe handles for its own `stdin` and `stdout`. This means that our parent process can basically hijack the child process's `stdin` and `stdout` for IPC, where normally these handles would be used for things like terminal output.

Pipes are one-way, so you actually want to create two of them: a main-to-monitor pipe, whose "read" handle will be the monitor process's `stdin`; and a monitor-to-main pipe, whose "write" handle will be the monitor process's `stdout`. As for actually sending data over a pipe? `ReadFile` can be given a pipe handle to read from, and `WriteFile` can be given a pipe handle to write to.

Something important to realize, though, is that pipes have an internal buffer, and if that buffer fills up, `WriteFile` will block until the recipient reads some data out of that buffer to make room. The initial design idea that I had (off the top of my head before I had time to read up on how these functions work) was to have the monitor process wait for a Win32 event to be signalled; upon intercepting a crash, the main process would send the error information and then signal the event. This would've been likely to deadlock, if the main process sent more error information than could fit in the pipe's internal buffer; the main process would block until the monitor process reads, but the monitor process would be waiting on the main process to signal that event.

No; your code needs to be able to send and receive data incrementally. Fortunately, it doesn't have to send and receive data in lockstep; since pipes have an internal buffer, the main process can send data before the monitor process has had a chance to start reading, and the monitor process will still see that data when it eventually does start reading.

RVT, then, has a collection of functions for IPC. Writing data involves static member functions that explicitly avoid any allocations (since we can't know whether the heap is safe to use), while reading data involves constructing the appropriate objects (since we're in a different process with a safe heap) and calling a `read` member function to pull the appropriate data.

`ReadFile` blocks until data is read or until an error occurs, so we can just have our monitor process attempt a read right off rip. The monitor process will wait until either the main process actually sends some data, or until the main process dies (such that the main-to-monitor write handle closes). The main process, meanwhile, can send all the data it needs to and then `ReadFile` a dummy byte, to wait until the minidump is done; the monitor process will send that byte when it's done minidumping the main process and notifying the user, and then the main process can finally let itself die.
