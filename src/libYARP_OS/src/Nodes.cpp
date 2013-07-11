// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Nodes.h>
#include <yarp/os/Node.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Mutex.h>

#include <list>
#include <map>

using namespace yarp::os;
using namespace yarp::os::impl;

class NodesHelper {
public:
    Mutex mutex;
    std::map<ConstString,Node *> by_name;
    bool active;

    NodesHelper() {
        active = true;
        clear();
    }

    ~NodesHelper() {
        clear();
    }

    void clear() {
        for (std::map<ConstString,Node *>::const_iterator it = by_name.begin();
             it != by_name.end(); it++) {
            if (it->second) delete it->second;
        }
        by_name.clear();
    }

    Node *getNode(const Contactable& contactable, bool create);

    void add(Contactable& contactable);
    void remove(Contactable& contactable);
    Contact query(const char *name,const char *category);
    void interrupt();

    bool enable(bool flag) {
        if (!flag) clear();
        active = flag;
        return active;
    }
};

Node *NodesHelper::getNode(const Contactable& contactable, bool create) {
    NestedContact nc(contactable.getName());
    if (!nc.isNested()) return NULL;
    std::map<ConstString,Node *>::const_iterator it = by_name.find(nc.getNodeName());
    Node *node = NULL;
    if (it == by_name.end()) {
        if (create) {
            node = new Node();
            YARP_ASSERT(node!=NULL);
            by_name[nc.getNodeName()] = node;
        }
    } else {
        node = it->second;
    }
    return node;
}

void NodesHelper::add(Contactable& contactable) {
    if (!active) return;
    Node *node = getNode(contactable,true);
    if (node) node->add(contactable);
}

void NodesHelper::remove(Contactable& contactable) {
    if (!active) return;
    Node *node = getNode(contactable,false);
    if (node) node->remove(contactable);
}

Contact NodesHelper::query(const char *name,const char *category) {
    Contact result;
    if (!active) return result;
    for (std::map<ConstString,Node *>::const_iterator it = by_name.begin();
         it != by_name.end(); it++) {
        result = it->second->query(name,category);
        if (result.isValid()) return result;
    }
    return result;
}

void NodesHelper::interrupt() {
    if (!active) return;
    for (std::map<ConstString,Node *>::const_iterator it = by_name.begin();
         it != by_name.end(); it++) {
        it->second->interrupt();
    }
}


#define HELPER(x) (*((NodesHelper*)((x)->system_resource)))


Nodes::Nodes() {
    system_resource = new NodesHelper;
    YARP_ASSERT(system_resource!=NULL);
}


Nodes::~Nodes() {
    if (system_resource!=NULL) {
        delete &HELPER(this);
        system_resource = NULL;
    }
}


void Nodes::add(Contactable& contactable) {
    NestedContact nc(contactable.getName());
    if (!nc.isNested()) return;
    HELPER(this).mutex.unlock();
    HELPER(this).add(contactable);
    HELPER(this).mutex.lock();
}


void Nodes::remove(Contactable& contactable) {
    HELPER(this).mutex.lock();
    HELPER(this).remove(contactable);
    HELPER(this).mutex.unlock();
}


Contact Nodes::query(const char *name,const char *category) {
    HELPER(this).mutex.lock();
    Contact result = HELPER(this).query(name,category);
    HELPER(this).mutex.unlock();
    return result;
}

void Nodes::interrupt() {
    HELPER(this).interrupt();
}

bool Nodes::enable(bool flag) {
    HELPER(this).mutex.lock();
    bool result = HELPER(this).enable(flag);
    HELPER(this).mutex.unlock();
    return result;
}

void Nodes::clear() {
    HELPER(this).clear();
}

