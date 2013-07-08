// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Node.h>
#include <yarp/os/Mutex.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/Port.h>
#include <yarp/os/Network.h>

#include <list>
#include <map>

using namespace yarp::os;
using namespace yarp::os::impl;

class NodeItem {
public:
    NestedContact nc;
    Contactable *contactable;
};

class NodeArgs {
public:
    Bottle request;
    Bottle args;
    Bottle reply;
    int code;
    ConstString msg;
};

class NodeHelper : public PortReader {
public:
    std::map<ConstString,NodeItem> by_part_name;
    std::multimap<ConstString,NodeItem> by_category;
    std::map<Contactable*,NodeItem> name_cache;
    Port port;
    Node *owner;

    Mutex mutex;
    ConstString name;

    NodeHelper() {
        owner = NULL;
        clear();
    }

    ~NodeHelper() {
        clear();
    }

    void clear() {
    }

    void add(Contactable& contactable);
    void remove(Contactable& contactable);
    Contact query(const char *name, const char *category);

    void interrupt() {
        port.interrupt();
    }

    virtual bool read(ConnectionReader& reader);

    void getBusStats(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void getBusInfo(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void getMasterUri(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void shutdown(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void getPid(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void getSubscriptions(NodeArgs& na) {
        // List resources with either no category or "-" category.
        // But how to determine types, for ROS purposes?
        na.reply.fromString("hmm");
    }

    void getPublications(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void paramUpdate(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void publisherUpdate(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void requestTopic(NodeArgs& na) {
        na.reply.fromString("hmm");
    }
};

void NodeHelper::add(Contactable& contactable) {
    NodeItem item;
    item.nc.fromString(contactable.getName());
    if (name=="") name = item.nc.getNodeName();
    if (name!=item.nc.getNodeName()) {
        fprintf(stderr,"Node name mismatch, expected [%s] but got [%s]\n",
                name.c_str(), item.nc.getNodeName().c_str());
        return;
    }
    if (port.getName()=="") {
        port.setReader(*this);
        port.open(name);
    }
    item.contactable = &contactable;
    name_cache[&contactable] = item;
    by_part_name[item.nc.getNestedName()] = item;
    by_category.insert(std::pair<ConstString,NodeItem>(item.nc.getCategory(),item));
}

void NodeHelper::remove(Contactable& contactable) {
    NodeItem item = name_cache[&contactable];
    name_cache.erase(&contactable);
    by_part_name.erase(item.nc.getNestedName());
    by_category.erase(item.nc.getCategory());
}

Contact NodeHelper::query(const char *name, const char *category) {
    Contact result = Contact::invalid();
    std::map<ConstString,NodeItem>::const_iterator i = by_part_name.find(name);
    if (i != by_part_name.end()) {
        result = i->second.contactable->where();
    }
    return result;
}

bool NodeHelper::read(ConnectionReader& reader) {
    NodeArgs na;
    na.request.read(reader);
    ConstString key = na.request.get(0).asString();
    na.args = na.request.tail().tail();
    na.code = -1;
    if (key=="getBusStats") {
        getBusStats(na);
    } else {
        na.reply.fromString("[fail]");
    }
    if (reader.getWriter()) {
        Bottle full;
        full.addInt(na.code);
        full.addString(na.msg);
        full.addList() = na.reply;
        full.write(*reader.getWriter());
    }
    return true;
}



#define HELPER(x) (*((NodeHelper*)((x)->system_resource)))

Node::Node() {
    system_resource = new NodeHelper;
    YARP_ASSERT(system_resource!=NULL);
    HELPER(this).owner = this;
}

Node::~Node() {
    if (system_resource!=NULL) {
        delete &HELPER(this);
        system_resource = NULL;
    }
}

void Node::add(Contactable& contactable) {
    HELPER(this).mutex.lock();
    HELPER(this).add(contactable);
    HELPER(this).mutex.unlock();
}

void Node::remove(Contactable& contactable) {
    HELPER(this).mutex.lock();
    HELPER(this).remove(contactable);
    HELPER(this).mutex.unlock();
}

Contact Node::query(const char *name,
                    const char *category) {
    HELPER(this).mutex.lock();
    Contact result = HELPER(this).query(name,category);
    HELPER(this).mutex.unlock();
    return result;
}


void Node::interrupt() {
    HELPER(this).interrupt();
}
