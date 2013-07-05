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

#include <list>
#include <map>

using namespace yarp::os;
using namespace yarp::os::impl;

class NodeItem {
public:
    std::string full_name;
    std::string part_name;
    std::string category;
    Contactable *contactable;
};

typedef std::list<NodeItem> NodeItemList;

class NodeHelper {
public:
    std::map<std::string,NodeItem> by_part_name;
    std::multimap<std::string,NodeItem> by_category;
    std::map<Contactable*,NodeItem> name_cache;

    Mutex mutex;

    NodeHelper() {
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
};

void NodeHelper::add(Contactable& contactable) {
    mutex.lock();
    NodeItem item;
    item.full_name = contactable.getName();
    item.contactable = &contactable;
    std::string::size_type idx = item.full_name.find("#");
    if (idx!=std::string::npos) {
        std::string node = item.full_name.substr(0,idx);
        // node should be a constant
        item.part_name = item.full_name.substr(idx+1,item.full_name.length());
    }
    item.category = "port";
    name_cache[&contactable] = item;
    by_part_name[item.part_name] = item;
    by_category.insert(std::pair<std::string,NodeItem>(item.category,item));
    mutex.unlock();
}

void NodeHelper::remove(Contactable& contactable) {
    mutex.lock();
    NodeItem item = name_cache[&contactable];
    name_cache.erase(&contactable);
    by_part_name.erase(item.part_name);
    by_category.erase(item.category);
    mutex.unlock();
}

Contact NodeHelper::query(const char *name, const char *category) {
    Contact result = Contact::invalid();
    mutex.lock();
    std::map<std::string,NodeItem>::const_iterator i = by_part_name.find(name);
    if (i != by_part_name.end()) {
        result = i->second.contactable->where();
    }
    mutex.unlock();
    return result;
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

Contact Node::query(const char *name,
                    const char *category) {
    return HELPER(this).query(name,category);
}

