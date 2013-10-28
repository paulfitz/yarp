// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPDB_ROSADAPTER
#define YARPDB_ROSADAPTER

#include <yarp/name/NameService.h>
#include "SubscriberOnSql.h"

namespace yarp {
    namespace name {
        class RosAdaptor;
    }
}

class yarp::name::RosAdaptor : public NameService {
private:
    yarp::name::NameService *store;
    void *implementation;
    yarp::os::Semaphore *mutex;
    bool verbose;
public:
    RosAdaptor() : store(0/*NULL*/), 
                   implementation(0/*NULL*/), 
                   mutex(0/*NULL*/),
                   verbose(true)
    {}

    void setStore(yarp::name::NameService& store) { this->store = &store; }
    yarp::name::NameService *getStore() { return store; }
    void setSubscriber(SubscriberOnSql *sub);

    virtual bool apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote);

    bool getSystemState(yarp::os::Bottle& cmd,
                        yarp::os::Bottle& reply);

    bool getTopicTypes(yarp::os::Bottle& cmd,
                       yarp::os::Bottle& reply);

    bool lookupNode(yarp::os::Bottle& cmd,
                    yarp::os::Bottle& reply);

    bool getPublishedTopics(yarp::os::Bottle& cmd,
                            yarp::os::Bottle& reply);

    bool registerPublisher(yarp::os::Bottle& cmd,
                           yarp::os::Bottle& reply);
};

#endif
