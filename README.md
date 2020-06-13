# What

This project is a cross-platform C++ high-performance timer.

It can be used to time code with high resolution (~1.5 usecs). This is useful for things like performance characterization or manual profiling. It can also be used to regulate game loops and other frequency-based events.

There is a C++98 version (Linux / Windows) as well as a standard C++11 version.

# Add to your project

This is a header-only project. You can simply download and include *PerformanceTimer98.hpp*.

It also comes with a simple CMake script that creates a target. If you prefer this, download the directory and add it in your own project's CMake script with `add_subdirectory(PerformanceTimer98)`. Then you can add it to executables or libraries with `target_link_libraries(<YourExec> PerformanceTimer98)`.

The license is BSD 3-clause. The license text is in the header file itself.

# How to use

#### Performance Measurement

```c++
PerformanceTimer98 timer;
timer.Start();  // Mark the start time.
// CRITICAL SECTION CODE TO MEASURE.

timer.Stop();  // Mark the stop time.
std::cout << "Elapsed time in milliseconds: " << timer.GetElapsed() << std::endl;
```

#### Game Loop

```C++
PerformanceTimer98 timer;
timer.SetInterval(60);  // 60 FPS.

timer.Start();

while (true)
{
    // GAME UPDATE / DRAWING CODE.

    for (timer.Stop(); !timer.IntervalHasElapsed(); timer.Stop())
    {
        if (timer.GetRemaining() > 1)  // See note.
            std::this_thread::yield();
    }
    timer.Start();
}
```

Note: Busy looping does consume CPU resources. You can execute `std::this_thread::yield()` to reduce CPU resource usage. However, this makes loop regulation less accurate, as the wait time before your process is scheduled again can vary and might be significant on this time scale. Try yielding until a small amount of time is left (1 ms in this example, but you'll have to tune it for yourself) before switching to a busy loop.

Note: Calling `Stop()` doesn't "stop" the timer--it merely records the current time as the stop time. So it's ok to call `Stop()` multiple times, because the elapsed time is measured from `Start()` to the most recent `Stop()`.

# Cross-platform issues

The C++11 standard introduced `std::chrono::high_performance_timer`. In the MSVC standard implementation, `high_performance_timer` is a type alias of `steady_clock`. After doing some testing, I discovered that the C++98 version, which uses `QueryPerformanceCounter`, is higher resolution than `high_performance_timer` on my Windows system. On Ubuntu, both versions performed about the same.

**Therefore, if high-performance timing is required, you should test both versions to discover which one has the better resolution.** For Windows, this is probably the C++98 version. If you are less concerned about resolution and just want a standard implementation, go with the C++11 version.
