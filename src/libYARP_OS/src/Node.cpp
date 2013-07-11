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

    void update() {
        if (nc.getTypeName()=="") {
            if (!contactable) return;
            Type typ = contactable->getType();
            if (typ.isValid()) {
                nc.setTypeName(typ.getName());
            }
        }
    }

    bool isSubscriber() {
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="-");
    }

    bool isPublisher() {
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="+");
    }

    bool isTopic() {
        ConstString cat = nc.getCategory();
        return (cat=="" || cat=="+" || cat=="-");
    }
};

class NodeArgs {
public:
    Bottle request;
    Bottle args;
    Bottle reply;
    int code;
    ConstString msg;
    bool should_drop;
    
    NodeArgs() {
        code = -1;
        should_drop = true;
    }

    void error(const char *txt) {
        msg = txt;
        code = -1;
    }

    void fail(const char *txt) {
        msg = txt;
        code = 0;
    }

    void success() {
        msg = "";
        code = 1;
    }
    
    void drop() {
        should_drop = true;
    }

    void persist() {
        should_drop = false;
    }

    void fromExternal(const Bottle& alt) {
        code = alt.get(0).asInt();
        msg = alt.get(1).asString();
        Bottle *nest = alt.get(2).asList();
        if (nest) reply = *nest;
    }
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

    Contact lookup(const ConstString& topic) {
        mutex.lock();
        std::map<ConstString,NodeItem>::const_iterator i = 
            by_part_name.find(topic);
        if (i == by_part_name.end()) {
            mutex.unlock();
            return Contact();
        }
        Contact c = i->second.contactable->where();
        mutex.unlock();
        return c;
    }

    void getBusStats(NodeArgs& na) {
        na.reply.addList();
        na.success();
    }

    void getBusInfo(NodeArgs& na) {
        na.reply.addList();
        na.success();
    }

    void getMasterUri(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void shutdown(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void getPid(NodeArgs& na) {
        na.reply.addInt(ACE_OS::getpid());
        na.success();
    }

    void getSubscriptions(NodeArgs& na) {
        mutex.lock();
        for (std::map<ConstString,NodeItem>::iterator it = by_part_name.begin(); it != by_part_name.end(); it++) {
            NodeItem& item = it->second;
            if (!item.isSubscriber()) continue;
            item.update();
            Bottle& lst = na.reply.addList();
            lst.addString(item.nc.getNestedName());
            lst.addString(item.nc.getTypeName());
        }
        mutex.unlock();
        na.success();
    }

    void getPublications(NodeArgs& na) {
        mutex.lock();
        for (std::map<ConstString,NodeItem>::iterator it = by_part_name.begin(); it != by_part_name.end(); it++) {
            NodeItem& item = it->second;
            if (!item.isPublisher()) continue;
            item.update();
            Bottle& lst = na.reply.addList();
            lst.addString(item.nc.getNestedName());
            lst.addString(item.nc.getTypeName());
        }
        mutex.unlock();
        na.success();
    }

    void paramUpdate(NodeArgs& na) {
        na.reply.fromString("hmm");
    }

    void publisherUpdate(NodeArgs& na) {
        ConstString topic = na.args.get(0).asString();
        Contact c = lookup(topic);
        if (!c.isValid()) {
            na.fail("Cannot find topic");
            return;
        }
        // just pass the message along, YARP ports know what to do with it
        ContactStyle style;
        style.admin = true;
        Bottle reply;
        if (!NetworkBase::write(c,na.request,reply,style)) {
            na.fail("Cannot communicate with local port");
            return;
        }
        na.fromExternal(reply);
    }

    void requestTopic(NodeArgs& na) {
        ConstString topic = na.args.get(0).asString();
        Contact c = lookup(topic);
        if (!c.isValid()) {
            na.fail("Cannot find topic");
            return;
        }
        na.reply.addString("TCPROS");
        na.reply.addString(c.getHost());
        na.reply.addInt(c.getPort());
        na.success();
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
    if (key=="getBusStats") {
        getBusStats(na);
    } else if (key=="getBusInfo") {
        getBusInfo(na);
    } else if (key=="getMasterUri") {
        getMasterUri(na);
    } else if (key=="shutdown") {
        shutdown(na);
    } else if (key=="getPid") {
        getPid(na);
    } else if (key=="getSubscriptions") {
        getSubscriptions(na);
    } else if (key=="getPublications") {
        getPublications(na);
    } else if (key=="paramUpdate") {
        paramUpdate(na);
    } else if (key=="publisherUpdate") {
        publisherUpdate(na);
    } else if (key=="requestTopic") {
        requestTopic(na);
    } else {
        na.error("I have no idea what you are talking about");
    }
    if (na.should_drop) {
        reader.requestDrop(); // ROS likes to close down.
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
