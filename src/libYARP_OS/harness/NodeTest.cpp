// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Node.h>
#include <yarp/os/Nodes.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/NameClient.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class NodeTest : public UnitTest {
public:
    virtual String getName() { return "NodeTest"; }

    void basicNodeTest();
    void basicNodesTest();

    virtual void runTests();
};

void NodeTest::basicNodeTest() {
    report(0,"most basic node test");
    NameClient::getNameClient().getNodes().enable(false);
    Node n;
    Port p1;
    Port p2;
    p1.open("/test#/p1");
    p2.open("/test#/p2");
    n.add(p1);
    n.add(p2);
    Contact c = n.query("/p1");
    checkTrue(c.isValid(),"found /p1");
    c = n.query("/p9");
    checkFalse(c.isValid(),"failed to find non-existent /p9");
    n.remove(p2);
    n.remove(p1);
}

void NodeTest::basicNodesTest() {
    report(0,"most basic nodes test");
    NameClient::getNameClient().getNodes().enable(false);
    Nodes n;
    Port p1;
    Port p2;
    Port p3;
    p1.open("/test#/p1");
    p2.open("/test#/p2");
    p3.open("/test2#/p3");
    n.add(p1);
    n.add(p2);
    n.add(p3);
    Contact c = n.query("/p1");
    checkTrue(c.isValid(),"found /p1");
    c = n.query("/p9");
    checkFalse(c.isValid(),"failed to find non-existent /p9");
    c = n.query("/p3");
    checkTrue(c.isValid(),"found /p3");

    Network::connect(n.query("/p1").getName(),n.query("/p3").getName());
    Network::sync(n.query("/p1").getName());
    Network::sync(n.query("/p3").getName());
    checkTrue(Network::isConnected("/test#/p1","/test2#/p3"),"good contacts");

    n.remove(p3);
    n.remove(p2);
    n.remove(p1);
}

void NodeTest::runTests() {
    NetworkBase::setLocalMode(true);
    basicNodeTest();
    basicNodesTest();
    NetworkBase::setLocalMode(false);
}

static NodeTest theNodeTest;

UnitTest& getNodeTest() {
    return theNodeTest;
}

