// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Node.h>
#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class NodeItem {
public:
};

typedef PlatformVector<NodeItem *> NodeItemList;

class NodeHelper {
public:
    NodeItemList items;

    NodeHelper() {
        clear();
    }

    ~NodeHelper() {
        clear();
    }

    void clear() {
        for (int i=0; i<(int)items.size(); i++) {
            NodeItem *it = items[i];
            if (it) {
                delete it;
                it = NULL;
            }
        }
        items.clear();
    }

    void add(Contactable& contactable);
    void remove(Contactable& contactable);
    Contact query(const char *name);
};

void NodeHelper::add(Contactable& contactable) {
}

void NodeHelper::remove(Contactable& contactable) {
}

Contact NodeHelper::query(const char *name) {
    return Contact();
}



#define HELPER(x) (*((NodeHelper*)((x)->system_resource)))

Node::Node() {
    system_resource = new NodeHelper;
    YARP_ASSERT(system_resource!=NULL);
}

Node::~Node() {
    if (system_resource!=NULL) {
        delete &HELPER(this);
        system_resource = NULL;
    }
}

void Node::add(Contactable& contactable) {
    HELPER(this).add(contactable);
}

void Node::remove(Contactable& contactable) {
    HELPER(this).remove(contactable);
}

Contact Node::query(const char *name) {
    return HELPER(this).query(name);
}

