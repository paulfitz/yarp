// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_NODE_
#define _YARP2_NODE_

#include <yarp/os/Port.h>

namespace yarp {
    namespace os {
        class Node;
    }
}

class YARP_OS_API yarp::os::Node {
public:
    yarp::os::Port port;
};

#endif
