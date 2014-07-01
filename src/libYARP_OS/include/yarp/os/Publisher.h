// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2014 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_PUBLISHER_
#define _YARP2_PUBLISHER_

#include <yarp/os/Log.h>
#include <yarp/os/UnbufferedContactable.h>
#include <yarp/os/BufferedPort.h>

namespace yarp {
    namespace os {
         template <class T> class Publisher;
    }
}

template <class T>
class yarp::os::Publisher : public UnbufferedContactable {
public:
    using Contactable::open;

    Publisher() {
        buffered_port = 0 /*NULL*/;
        T example;
        port.promiseType(example.getType());
        port.setInputMode(false);
        port.setOutputMode(true);
        port.setRpcMode(false);
    }

    virtual ~Publisher() {
        clear();
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
    virtual bool addOutput(const ConstString& name) {
        return port.addOutput(name);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const ConstString& name, 
                           const ConstString& carrier) {
        return port.addOutput(name,carrier);
    }

    // documentation provided in Contactable
    virtual bool addOutput(const Contact& contact) {
        return port.addOutput(contact);
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

    // documentation provided in Contactable
    virtual Contact where() const {
        return port.where();
    }

    // documentation provided in Contactable
    virtual ConstString getName() const {
        return port.getName();
    }

    // documented in UnbufferedContactable
    virtual bool write(PortWriter& writer, 
                       PortWriter *callback = 0 /*NULL*/) const {
        return port.write(writer,callback);
    }

    // documented in UnbufferedContactable
    virtual bool write(PortWriter& writer, PortReader& reader,
                       PortWriter *callback = 0 /*NULL*/) const {
        return port.write(writer,reader,callback);
    }

    // documented in UnbufferedContactable
    virtual bool read(PortReader& reader, bool willReply = false) {
        return false;
    }

    // documented in UnbufferedContactable
    virtual bool reply(PortWriter& writer) {
        return false;
    }

    // documented in UnbufferedContactable
    virtual bool replyAndDrop(PortWriter& writer) {
        return false;
    }

    // documented in Contactable
    virtual bool setEnvelope(PortWriter& envelope) {
        return port.setEnvelope(envelope);
    }

    // documented in Contactable
    virtual bool getEnvelope(PortReader& envelope) {
        return port.getEnvelope(envelope);
    }

    // documented in Contactable
    virtual int getInputCount() {
        return port.getInputCount();
    }

    // documented in Contactable
    virtual int getOutputCount() {
        return port.getOutputCount();
    }

    // documented in Contactable
    virtual void getReport(PortReport& reporter) {
        port.getReport(reporter);
    }

    // documented in Contactable
    virtual void setReporter(PortReport& reporter) {
        port.setReporter(reporter);
    }

    // documented in Contactable
    virtual bool isWriting() {
        return active().isWriting();
    }

    // documented in Contactable
    void setReader(PortReader& reader) {
        active().setReader(reader);
    }

    virtual Type getType() {
        port.getType();
    }

    virtual void promiseType(const Type& typ) {
        port.promiseType(typ);
    }

    virtual Property *acquireProperties(bool readOnly) {
        return port.acquireProperties(readOnly);
    }

    virtual void releaseProperties(Property *prop) {
        port.releaseProperties(prop);
    }

    void setInputMode(bool expectInput) {
        port.setInputMode(expectInput);
    }

    void setOutputMode(bool expectOutput) {
        port.setOutputMode(expectOutput);
    }

    void setRpcMode(bool expectRpc) {
        port.setRpcMode(expectRpc);
    }

    T& prepare() {
        return buffer().prepare();
    }

    void write(bool forceStrict=false) {
        buffer().write(forceStrict);
    }

    void waitForWrite() {
        buffer().waitForWrite();
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

