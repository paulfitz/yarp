// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/StringOutputStream.h>

#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class BufferedConnectionWriterTest : public UnitTest {
public:
    virtual String getName() { return "BufferedConnectionWriterTest"; }

    void testWrite() {
        report(0,"testing writing...");
        StringOutputStream sos;    
        BufferedConnectionWriter bbr;
        bbr.reset(true);
        bbr.appendLine("Hello");
        bbr.appendLine("Greetings");
        bbr.write(sos);
        checkEqual(sos.toString(),"Hello\r\nGreetings\r\n","two line writes");
    }

    void testRestart() {
        report(0,"test restarting writer without reallocating memory...");
        StringOutputStream sos;

        // first we test a message with a few short strings
        BufferedConnectionWriter bbr;
        bbr.reset(true);
        ConstString msg1("Hello");
        ConstString msg2("Greetings");
        heapMonitorBegin();
        bbr.appendLine(msg1);
        bbr.appendLine(msg2);
        int ops = heapMonitorEnd();
        bbr.write(sos);
        checkEqual(sos.toString(),"Hello\r\nGreetings\r\n","two line writes");
        if (heapMonitorSupported()) {
            checkTrue(ops>0,"memory allocation happened");
        }
        sos.reset();
        heapMonitorBlock();
        bbr.restart();
        bbr.appendLine(msg1);
        bbr.appendLine(msg2);
        heapMonitorEnd();
        bbr.write(sos);
        checkEqual(sos.toString(),"Hello\r\nGreetings\r\n","two line writes dup");

        // Make sure we survive a small change in message
        bbr.restart();
        sos.reset();
        bbr.appendLine("Space Monkeys");
        bbr.appendLine("Attack");
        bbr.write(sos);
        checkEqual(sos.toString(),"Space Monkeys\r\nAttack\r\n","alternate text");

        // And again, a bigger change this time
        String test(2024,'x');
        bbr.restart();
        sos.reset();
        bbr.appendLine(test);
        bbr.appendLine(test);
        bbr.appendLine(test);
        bbr.write(sos);
        ConstString result = sos.toString();
        ConstString expect = test + "\r\n" + test + "\r\n" + test + "\r\n";
        checkEqual(result,expect,"long text");
        sos.reset();
        heapMonitorBlock();
        bbr.restart();
        bbr.appendLine(test);
        bbr.appendLine(test);
        bbr.appendLine(test);
        heapMonitorEnd();
        bbr.write(sos);
        result = sos.toString();
        checkEqual(result,expect,"long text, take 2");
    }

    virtual void runTests() {
        testWrite();
        testRestart();
    }
};

static BufferedConnectionWriterTest theBufferedConnectionWriterTest;

UnitTest& getBufferedConnectionWriterTest() {
    return theBufferedConnectionWriterTest;
}

