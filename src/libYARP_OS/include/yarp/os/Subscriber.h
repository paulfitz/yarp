// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SUBSCRIBER_
#define _YARP2_SUBSCRIBER_

#include <yarp/os/Log.h>
#include <yarp/os/AbstractContactable.h>
#include <yarp/os/BufferedPort.h>

namespace yarp {
    namespace os {
         template <class T> class Subscriber;
    }
}

template <class T>
class yarp::os::Subscriber : public AbstractContactable {
public:
    using Contactable::open;

    Subscriber(const ConstString& name = "") {
        buffered_port = 0 /*NULL*/;
        T example;
        port.promiseType(example.getType());
        port.setInputMode(true);
        port.setOutputMode(false);
        port.setRpcMode(false);
        if (name!="") {
            YARP_ASSERT(topic(name));
        }
    }

    virtual ~Subscriber() {
        clear();
    }

    bool topic(const ConstString& name) {
        port.includeNodeInName(true);
        return open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const ConstString& name) {
        clear();
        return port.open(name);
    }

    // documentation provided in Contactable
    virtual bool open(const Contact& contact, bool registerName = true) {
        clear();
        return port.open(contact,registerName);
    }

    // documentation provided in Contactable
    virtual void close() {
        active().close();
    }

    // documentation provided in Contactable
    virtual void interrupt() {
        active().interrupt();        
    }

    // documentation provided in Contactable
    virtual void resume() {
        active().resume();        
    }

    // documented in Contactable
    void setReader(PortReader& reader) {
        active().setReader(reader);
    }

    T *read(bool wait = true) {
        return buffer().read(wait);
    }

    virtual Port& asPort() {
        return port;
    }

    virtual const Port& asPort() const {
        return port;
    }
    
private:
    Port port;
    BufferedPort<T> *buffered_port;

    Contactable& active() {
        if (buffered_port) return *buffered_port;
        return port;
    }

    BufferedPort<T>& buffer() {
        if (!buffered_port) {
            buffered_port = new BufferedPort<T>(port);
            YARP_ASSERT(buffered_port);
        }
        return *buffered_port;
    }

    void clear() {
        if (!buffered_port) return;
        delete buffered_port;
        buffered_port = 0 /*NULL*/;
    }
};

#endif

