// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_CLOCKTIME_
#define _YARP2_CLOCKTIME_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Clock;
    }
}

class YARP_OS_API yarp::os::Clock {
public:
    ~Clock() {}
    virtual double now() = 0;
    virtual void delay(double seconds) = 0;
    virtual bool isValid() const = 0;
};

#endif
