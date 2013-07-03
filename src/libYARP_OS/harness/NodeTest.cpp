// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Node.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class NodeTest : public UnitTest {
public:
    virtual String getName() { return "NodeTest"; }

    void basicTest();

    virtual void runTests();
};

void NodeTest::basicTest() {
    report(0,"most basic node test");
    Node n;
    Port p1;
    Port p2;
    p1.open("/test#/p1");
    p2.open("/test#/p2");
    n.add(p1);
    n.add(p2);
    n.remove(p2);
    n.remove(p1);
}

void NodeTest::runTests() {
    NetworkBase::setLocalMode(true);
    basicTest();
    NetworkBase::setLocalMode(false);
}

static NodeTest theNodeTest;

UnitTest& getNodeTest() {
    return theNodeTest;
}

