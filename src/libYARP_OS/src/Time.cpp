// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Time.h>
#include <yarp/os/impl/PlatformTime.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/NetworkClock.h>
#include <yarp/os/Network.h>
#include <yarp/os/Log.h>

#ifdef ACE_WIN32
// for WIN32 MM functions
#include <mmsystem.h>
#endif

using namespace yarp::os;

static SystemClock system_clock;
static Clock *pclock = NULL;
static bool clock_owned = false;
static ConstString network_clock_name = "";
static bool network_clock_pending = false;

static void removeClock() {
    if (pclock) {
        if (clock_owned) {
            delete pclock;
            clock_owned = false;
        }
        pclock = NULL;
    }
    network_clock_name = "";
    network_clock_pending = false;
}

static Clock& getClock() {
    if (network_clock_pending) {
        ConstString name;
        NetworkClock *nc = NULL;
        NetworkBase::lock();
        if (network_clock_pending) {
            name = network_clock_name;
            removeClock();
            network_clock_pending = false;
            pclock = nc = new NetworkClock();
            clock_owned = true;
            YARP_ASSERT(pclock);
        }
        NetworkBase::unlock();
        if (nc) {
            nc->open(name);
        }
    }
    if (pclock) return *pclock;
    return system_clock;
}

void Time::delay(double seconds) {
    getClock().delay(seconds);
}

double Time::now() {
    return getClock().now();
}

void Time::turboBoost() {
#ifdef ACE_WIN32
    // only does something on Microsoft Windows
    TIMECAPS tm;
    timeGetDevCaps(&tm, sizeof(TIMECAPS));
    timeBeginPeriod(tm.wPeriodMin);
#endif
}

void Time::yield() {
#ifdef YARP_HAS_ACE
    ACE_Time_Value tv(0);
    ACE_OS::sleep(tv);
#else
    sleep(0);
#endif
}


void Time::useSystemClock() {
    NetworkBase::lock();
    removeClock();
    NetworkBase::unlock();
}

void Time::useNetworkClock(const ConstString& clock) {
    NetworkBase::lock();
    removeClock();
    network_clock_name = clock;
    network_clock_pending = true;
    NetworkBase::unlock();
}

void Time::useCustomClock(Clock *clock) {
    NetworkBase::lock();
    removeClock();
    pclock = clock;
    YARP_ASSERT(pclock);
    NetworkBase::unlock();
}

bool Time::isSystemClock() {
    return (pclock==NULL);
}

bool Time::isValid() {
    return getClock().isValid();
}

