// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_STRINGOUTPUTSTREAM_
#define _YARP2_STRINGOUTPUTSTREAM_

#include <yarp/os/OutputStream.h>
#include <yarp/os/ConstString.h>

namespace yarp {
    namespace os {
        namespace impl {
            class StringOutputStream;
        }
    }
}

/**
 * An OutputStream that produces a String.  For testing purposes.
 */
class yarp::os::impl::StringOutputStream : public yarp::os::OutputStream {
public:
    using yarp::os::OutputStream::write;

    StringOutputStream() { }


    yarp::os::ConstString toString() {
        return data;
    }

    void reset() {
        data = "";
    }


    virtual void write(const Bytes& b) { // throws
        yarp::os::ConstString tmp;
        //YARP_STRSET(tmp,(char*)b.get(),b.length(),0);
        tmp = yarp::os::ConstString((char*)b.get(),b.length());
        data += tmp;
    }

    virtual void close() {
    }

    virtual bool isOk() { 
        return true;
    }

    const yarp::os::ConstString& str() { return data; }

private:
    yarp::os::ConstString data;
};

#endif

