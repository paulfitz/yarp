// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Nodes.h>
#include <yarp/os/Node.h>
#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::os;
using namespace yarp::os::impl;

typedef PlatformVector<Node *> NodeList;

class NodesHelper {
public:
    NodeList nodes;

    NodesHelper() {
        clear();
    }

    ~NodesHelper() {
        clear();
    }

    void clear() {
        for (int i=0; i<(int)nodes.size(); i++) {
            Node *n = nodes[i];
            if (n) {
                delete n;
                n = NULL;
            }
        }
        nodes.clear();
    }
};


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
