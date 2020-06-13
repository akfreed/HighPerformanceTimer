// ==================================================================
// BSD 3-Clause License
//
// Copyright (c) 2017-2020, Alexander K. Freed
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ==================================================================

// Language: ISO C++11

// This may not be the highest resolution timer on Windows. Compare with the C++98 version.

#pragma once

#include <cassert>
#include <chrono>
#include <type_traits>

//! A C++11 standard high-performance timer that can be used for accurately
//! tracking run time or controlling game loops.
class PerformanceTimer11
{
    using Clock = typename std::conditional<std::chrono::high_resolution_clock::is_steady,
        std::chrono::high_resolution_clock, std::chrono::steady_clock>::type;
    using Seconds      = std::chrono::duration<double>;
    using Milliseconds = std::chrono::duration<double, std::milli>;

public:
    PerformanceTimer11() = default;

    //! Since the clocks are standard now, this class will always work. However, it may
    //! not be high-resolution on all systems.
    //! @return true always.
    bool IsSupportedPlatform() const
    {
        return true;
    }

    //! @return The (optional) interval for managing loop timing. Unit is seconds.
    double GetInterval() const
    {
        return m_interval.count();
    }

    //! Set the (optional) interval for managing loop timing.
    //! Unit is ticks-per-second. e.g. 60 will set the interval to 1/60th of a second.
    //! @param[in] tickPerSecond The desired number of intervals per second.
    void SetInterval(double ticksPerSecond)
    {
        if (ticksPerSecond == 0)
        {
            assert(false);
            return;
        }
        m_interval = Seconds(1) / ticksPerSecond;
    }

    //! Mark the current time as the start point and stop point.
    void Start()
    {
        m_startTime = Clock::now();
        m_stopTime = m_startTime;
    }

    //! Mark the current time as the stop point.
    //! (Doesn't actually "stop" the timer--just sets the stop point.)
    void Stop()
    {
        m_stopTime = Clock::now();
    }

    //! @return The elapsed time from start to stop in milliseconds.
    double GetElapsed() const
    {
        return Milliseconds(m_stopTime - m_startTime).count();
    }

    //! @return The remaining time in the time interval in milliseconds. (i.e. interval - elapsed)
    double GetRemaining() const
    {
        return (m_interval - Milliseconds(m_stopTime - m_startTime)).count();
    }

    //!@return true if the time between start and stop is greater than the interval.
    bool IntervalHasElapsed() const
    {
        return m_stopTime - m_startTime >= m_interval;
    }

private:
    Clock::time_point m_startTime;
    Clock::time_point m_stopTime;
    Seconds           m_interval = Seconds(1) / 60;  // Default is 1/60th of a second.
};
