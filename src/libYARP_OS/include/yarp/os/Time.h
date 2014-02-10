// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_TIME_
#define _YARP2_TIME_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Time;
    }
}

/**
 * \ingroup key_class
 *
 * Services related to time -- delay, current time.
 */
class YARP_OS_API yarp::os::Time {
public:
    /**
     * Wait for a certain number of seconds.  This may be fractional.
     * @param seconds the duration of the delay, in seconds
     */
    static void delay(double seconds);

    /**
     * Return the current time in seconds, relative to an arbitrary 
     * starting point, using the best possible timer resolution.
     * @return the system time in seconds
     */
    static double now();

    /**
     * The calling thread releases its remaining quantum upon calling
     * this function.
     */
    static void yield();

    /**
     * For OS where it makes sense sets the scheduler to be called more often.
     * This sets the scheduler to be run to the maximum possible rate based
     * on the capability of the hardware.
     * Specifically, on Microsoft Windows, high resolution scheduling is
     * used.
     */  
    static void turboBoost();


    /*
    // extensions to support simulated time.  When practical, 
    // we model ROS's ros::Time API.

    /**
     *
     * Determine whether system time has been replaced with a simulation.
     *
     */
    //static bool isSimTime();

    /**
     *
     * Determine whether we are using unmodified system time.
     *
     */
    //static bool isSystemTime();

    /**
     *
     * Determine whether the current time is non-zero.  Time is zero
     * in the case of simulated time when a simulated time signal
     * has not yet been received.
     *
     */
    //static bool isValid();
    

};

#endif
