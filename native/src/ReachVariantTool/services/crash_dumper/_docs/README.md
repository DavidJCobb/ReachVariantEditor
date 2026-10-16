
# Crash dumper

This subsystem [catches crashes](./how%20to%20intercept%20a%20crash.md), notifies the user, and [saves minidumps](./how%20to%20save%20a%20minidump.md) that can be sent to me to let me inspect the program state in a debugger.

The `core` singleton outsources its work to one of two other singletons, depending on whether we're in the parent process or the child process: `main_process_state` or `monitor_process_state`.

## Notes

### Debugging

If the debugger is attached to RVT at the time a crash occurs, the debugger will intercept the crash before RVT itself can intercept that crash. This is generally good, except that it's a problem if you're specifically testing RVT's ability to intercept crashes. To run the "crash test" testcases, you'll want to detach the debugger before triggering the crash.

The [Microsoft Child Process Debugging Power Tool](https://marketplace.visualstudio.com/items?itemName=vsdbgplat.MicrosoftChildProcessDebuggingPowerTool) is an official Visual Studio add-on that can allow you to debug child processes spawned by a program you're debugging. This can be used to debug the monitor process (use the "Processes" debug window to view all processes; detach from the parent process so you can trigger our crash handling code; stay attached to the child process).

### `std::terminate`

The `std::set_terminate` function has non-standards-conforming behavior in MSVC. There is supposed to be just one global `std::terminate_handler`, but under MSVC, each thread has its own terminate handler; there is no way to set a global handler. Microsoft has been aware of this issue [since 2019 at the latest](https://developercommunity.visualstudio.com/t/c-stdset-terminate-does-not-syncronize-and-effect/507132). They've expressed a reluctance to fix it due to backwards-compatibility issues, they've made no apparent attempt to offer a non-standard API that implements the standard behavior, and they've offered no workarounds.

As such, I have no other option but to edit every piece of multi-threaded code in DovahKit to set the same terminate handler manually, and I have to also pray that nothing in any of these libraries spawns threads that might fail.

The frontend relies on `dovahkit::subsystems::crash_dumper::register_new_thread()` to set the terminate handler for its own threads. I don't want the backend to have to "reach into" the frontend, so the backend has all of its threads set a common terminate handler via `dovah::worker_thread_termination_handler`, and the frontend just customizes that.