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
    void basicTypeTest();
    void builtinNodeTest();
    void basicApiTest();
    void portTopicCombo();

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
    NameClient::getNameClient().getNodes().enable(true);
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
    NameClient::getNameClient().getNodes().enable(true);
}

void NodeTest::basicTypeTest() {
    report(0,"types coevolving with nodes");
    Bottle b;
    Type typ = b.getType();
    checkFalse(typ.hasName(),"Bottle type is not named yet");
}

void NodeTest::builtinNodeTest() {
    report(0,"check that auto node works");
    NameClient::getNameClient().getNodes().clear();
    {
        checkFalse(NetworkBase::exists("/test"),"node does not exist yet");
        Port p1;
        Port p2;
        p1.open("/test#/p1");
        p2.open("/test#/p2");
        checkTrue(NetworkBase::exists("/test"),"node exists now");
    }
    NameClient::getNameClient().getNodes().clear();
}

void NodeTest::basicApiTest() {
    report(0,"check node api");
    NameClient::getNameClient().getNodes().clear();
    Port p1;
    Port p2;
    p1.open("/test#/p1");
    p2.open("/test#/p2");
    Bottle cmd, reply;
    cmd.fromString("getSubscriptions dummy");
    NetworkBase::write(Contact::byName("/test"),cmd,reply);
    checkEqual(reply.get(0).asInt(),1,"getSubscriptions api success");
    cmd.fromString("requestTopic dummy /p2 (TCPROS)");
    NetworkBase::write(Contact::byName("/test"),cmd,reply);
    //printf("got %s\n", reply.toString().c_str());
    checkEqual(reply.get(0).asInt(),1,"found /p2");
    cmd.fromString("requestTopic dummy /p3 (TCPROS)");
    NetworkBase::write(Contact::byName("/test"),cmd,reply);
    checkEqual(reply.get(0).asInt(),0,"failed to find /p3");
    p1.close();
    p2.close();
    NameClient::getNameClient().getNodes().clear();
}


void NodeTest::portTopicCombo() {
    /*

      // hold off, until ancient test name server is replaced with
      // sqlite based name server (capabile of managing topics)

    report(0,"check port node combo");
    Port p1;
    Port p2;
    p1.open("/test#+/p1");
    p2.open("/test#-/p2");
    checkTrue(NetworkBase::isConnected(p1.getName(),
                                       p2.getName()), "auto connect working");
    */
}

void NodeTest::runTests() {
    NetworkBase::setLocalMode(true);
    basicNodeTest();
    basicNodesTest();
    basicTypeTest();
    builtinNodeTest();
    basicApiTest();
    portTopicCombo();
    NetworkBase::setLocalMode(false);
}

static NodeTest theNodeTest;

UnitTest& getNodeTest() {
    return theNodeTest;
}

