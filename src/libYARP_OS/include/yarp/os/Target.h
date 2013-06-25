// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_TARGET_
#define _YARP2_TARGET_

#include <yarp/os/api.h>

namespace yarp {
    namespace os {
        class Target;
    }
}

class YARP_OS_API yarp::os::Target {
public:

    /**
     * Destructor.
     */
    virtual ~Target() {
    }

    /**
     * Get the name associated with this contact.
     * @return The name associated with this contact, or the empty string
     *         if no name is set
     */
    virtual ConstString getRegName() const = 0;
    
    /**
     * Get the host name associated with this contact for socket communication.
     * @return The host name associated with this contact, or the empty string
     *         if no host name is set
     */
    virtual ConstString getHost() const = 0;

    /**
     * Get the carrier associated with this contact for socket communication.
     * @return The carrier associated with this contact, or the empty string
     *         if no carrier is set
     */
    virtual ConstString getCarrier() const = 0;


    /**
     * Get the port number associated with this contact for socket communication.
     * @return The port number associated with this contact, or <= 0
     *         if no port number is set
     */
    virtual int getPort() const = 0;

    /**
     * Get a textual representation of the contact.
     * @return a textual representation of the contact.
     */
    virtual ConstString toString() const = 0;

    /**
     * Checks if a contact is tagged as valid.
     * @return true iff contact is tagged as valid.
     */
    virtual bool isValid() const = 0;
};

#endif
