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

// Language: ISO C++98

#ifndef PERFORMANCETIMER98_H
#define PERFORMANCETIMER98_H


#if defined(_WIN32)

// ===========================================================================
// The Windows version

#include <windows.h>

#include <cassert>

//! A cross-platform high-performance timer that can be used for accurately
//! tracking run time or controlling game loops.
//! It works on Windows and Linux.
class PerformanceTimer98
{
public:
    PerformanceTimer98()
    {
        m_valid = QueryPerformanceFrequency(&m_perSecond);
        m_startTime.QuadPart = 0;
        m_stopTime.QuadPart = 0;
        m_interval = m_perSecond.QuadPart / 60.0;  // Default is 1/60th of a second.
        m_perMillisecond = m_perSecond.QuadPart / 1000.0;
    }

    //! Some Windows systems are not supported.
    //! @return true if this system is supported.
    bool IsSupportedPlatform() const
    {
        return m_valid;
    }

    //! @return The (optional) interval for managing loop timing. Unit is seconds.
    double GetInterval() const
    {
        return m_interval / m_perSecond.QuadPart;
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
        m_interval = m_perSecond.QuadPart / ticksPerSecond;
    }

    //! Mark the current time as the start point and stop point.
    void Start()
    {
        assert(IsSupportedPlatform());
        QueryPerformanceCounter(&m_startTime);
        m_stopTime = m_startTime;
    }

    //! Mark the current time as the stop point.
    //! (Doesn't actually "stop" the timer--just sets the stop point.)
    void Stop()
    {
        QueryPerformanceCounter(&m_stopTime);
        assert(IsSupportedPlatform());
    }

    //! @return The elapsed time from start to stop in milliseconds.
    double GetElapsed() const
    {
        return (m_stopTime.QuadPart - m_startTime.QuadPart) / m_perMillisecond;
    }

    //! @return The remaining time in the time interval in milliseconds. (i.e. interval - elapsed)
    double GetRemaining() const
    {
        return (m_interval + m_startTime.QuadPart - m_stopTime.QuadPart) / m_perMillisecond;
    }

    //!@return true if the time between start and stop is greater than the interval.
    bool IntervalHasElapsed() const
    {
        return (m_stopTime.QuadPart - m_startTime.QuadPart) >= m_interval;
    }

private:
    LARGE_INTEGER m_perSecond;
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_stopTime;
    double m_perMillisecond;
    double m_interval;
    bool   m_valid;
};


#elif defined (__linux__) || defined(__posix__)

// ===========================================================================
// The Linux version

#include <time.h>
#include <sys/time.h>

#include <cstring>
#include <cassert>

//! A cross-platform high-performance timer that can be used for accurately
//! tracking run time or controlling game loops.
//! It works on Windows and Linux.
class PerformanceTimer98
{
public:
    PerformanceTimer98()
        : m_interval(1 / 60.0)  // Default is 1/60th of a second.
    {
        std::memset(&m_startTime, 0, sizeof(m_startTime));
        std::memset(&m_stopTime, 0, sizeof(m_startTime));
    }

    //! Works on all Linux systems.
    //! This function is included to make it compatible with the Windows PerformanceTimer API.
    //! @return true always.
    bool IsSupportedPlatform() const
    {
        return true;
    }

    //! @return The (optional) interval for managing loop timing. Unit is seconds.
    double GetInterval() const
    {
        return m_interval;
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
        m_interval = 1 / ticksPerSecond;
    }

    //! Mark the current time as the start point and stop point.
    void Start()
    {
        gettimeofday(&m_startTime, NULL);
        m_stopTime = m_startTime;
    }

    //! Mark the current time as the stop point.
    //! (Doesn't actually "stop" the timer--just sets the stop point.)
    void Stop()
    {
        gettimeofday(&m_stopTime, NULL);
    }

    //! @return The elapsed time from start to stop in milliseconds.
    double GetElapsed() const
    {
        timeval diff;
        timersub(&m_stopTime, &m_startTime, &diff);
        return static_cast<double>(diff.tv_sec) * 1000 + static_cast<double>(diff.tv_usec) / 1000.0;
    }

    //! @return The remaining time in the time interval in milliseconds. (i.e. interval - elapsed)
    double GetRemaining() const
    {
        return m_interval * 1000 - GetElapsed();
    }

    //!@return true if the time between start and stop is greater than the interval.
    bool IntervalHasElapsed() const
    {
        timeval diff;
        timersub(&m_stopTime, &m_startTime, &diff);
        return (static_cast<double>(diff.tv_sec) + static_cast<double>(diff.tv_usec) / 1000000.0) >= m_interval;
    }

private:
    timeval m_startTime;
    timeval m_stopTime;
    double  m_interval;
};


#endif  // defined (__linux__) || defined(__posix__)

#endif  // PERFORMANCETIMER98_H
