//
// Copyright (c) 2007 Tridium, Inc.
// Licensed under the Academic Free License version 3.0
//
// History:
//   8 May 07  Brian Frank  Creation
//

#include "../svm/sedona.h"
#include <windows.h>

#define TICKS_ROLLOVER 4294967296

#ifdef UNIT_TESTS
    static DWORD sleepTicksMs = 0;
    void SleepUnitTests(DWORD dwMilliseconds);
    DWORD GetTickCountUnitTests();
    void ResetTicksUnitTests();
#endif // UNIT_TESTS

// static Str Sys.platformType()
Cell sys_Sys_platformType(SedonaVM* vm, Cell* params)
{
  Cell result;

#ifdef PLATFORM_TYPE
  result.aval = PLATFORM_TYPE;
#else
  result.aval = "sys::PlatformService";
#endif

  return result;
}

// static long Sys.ticks()
int64_t sys_Sys_ticks(SedonaVM* vm, Cell* params)
{
  static BOOL init = FALSE;
  static CRITICAL_SECTION lock;
  static int64_t rolloverTicks = 0;
  static DWORD lastTicks = 0;
  int64_t total = 0;
  DWORD ticks;

  // check for initialization
  if (!init)
  {
    InitializeCriticalSection(&lock);
    init = TRUE;
  }

  // grab lock
  EnterCriticalSection(&lock);

  // get the ticks and check for a rollover
  #ifdef UNIT_TESTS
    ticks = GetTickCountUnitTests();
    //ResetTicksUnitTests();
  #else
    ticks = GetTickCount();
  #endif // UNIT_TESTS


  if (ticks < lastTicks)
    rolloverTicks += TICKS_ROLLOVER;

  // save last ticks, and add our rollover
  lastTicks = ticks;
  total = rolloverTicks + ticks;

  // release lock
  LeaveCriticalSection(&lock);

  // milliseconds -> nanoseconds
  return total * 1000000ll;
}

// static void Sys.sleep(Time t)
Cell sys_Sys_sleep(SedonaVM* vm, Cell* params)
{
  int64_t ns = *(int64_t*)params;
  int64_t ms = ns/1000000ll;

  if (ms <= 0) return nullCell;

  #ifdef UNIT_TESTS
    SleepUnitTests(ms);
  #else
    Sleep(ms);
  #endif // UNIT_TESTS
  return nullCell;
}

#ifdef UNIT_TESTS
    void SleepUnitTests(DWORD dwMilliseconds)
    {
        sleepTicksMs += dwMilliseconds;
    }

    DWORD GetTickCountUnitTests()
    {
        return sleepTicksMs;
    }

    void ResetTicksUnitTests()
    {
        sleepTicksMs = 0;
    }
#endif // UNIT_TESTS


