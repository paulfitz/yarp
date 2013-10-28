// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "RosAdaptor.h"

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"

#include <map>
#include <list>
#include <string>

using namespace std;
using namespace yarp::os;
using namespace yarp::name;

/*

  Adaptor strategy:
  NODES get registered as PORTS (they talk XMLRPC just fine after all)
  PUBLISHERS etc are also registered as PORTS, with appropriate markers.

 */

#define SQLDB(x) ((sqlite3*)(x))


void RosAdaptor::setSubscriber(SubscriberOnSql *sub) {
    implementation = sub->getDatabase();
    mutex = sub->getDatabaseMutex();
    sqlite3 *db = SQLDB(implementation);


    const char *create_rosmaster_table = "CREATE TABLE IF NOT EXISTS rosmaster (\n\
    id INTEGER PRIMARY KEY,\n\
	node TEXT,\n\
	element TEXT,\n\
    role TEXT,\n\
	typ TEXT,\n\
    uri TEXT,\n\
    comment TEXT);";

    int result = sqlite3_exec(db, create_rosmaster_table, NULL, NULL, NULL);
    if (result!=SQLITE_OK) {
        sqlite3_close(db);
        fprintf(stderr,"Failed to set up structures table\n");
        exit(1);
    }
}

static void addStateGroup(Bottle& bot, map<string, list<string> >& data) {
    bot.addString("list");
    for (map<string, list<string> >::const_iterator it = data.begin();
         it != data.end(); it++) {
        Bottle& part = bot.addList();
        part.addString("list");
        part.addString(it->first.c_str());
        Bottle& tail = part.addList();
        tail.addString("list");
        const list<string>& subparts = it->second;
        for (list<string>::const_iterator it = subparts.begin();
             it != subparts.end(); it++) {
            tail.addString(it->c_str());
        }
    }
}


bool RosAdaptor::getSystemState(yarp::os::Bottle& cmd,
                                yarp::os::Bottle& reply) {
    mutex->wait();
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    query = sqlite3_mprintf("SELECT node, element, role, typ FROM rosmaster");
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                    NULL);
    ConstString sresult;
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
    }
    
    map<string, list<string> > publishers;
    map<string, list<string> > subscribers;
    map<string, list<string> > services;
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *node = (char *)sqlite3_column_text(statement,0);
        char *element = (char *)sqlite3_column_text(statement,1);
        char *role = (char *)sqlite3_column_text(statement,2);
        char *typ = (char *)sqlite3_column_text(statement,3);
        if (node==NULL||element==NULL||role==NULL) continue;
        string snode = node;
        string selement = element;
        string srole = role;
        if (srole=="publisher") {
            if (publishers.find(selement)==publishers.end()) {
                publishers[selement] = list<string>();
            }
            publishers[selement].push_back(snode);
        } else if (srole=="subscriber") {
            if (subscribers.find(selement)==subscribers.end()) {
                subscribers[selement] = list<string>();
            }
            subscribers[selement].push_back(snode);
        } else if (srole=="service") {
            if (services.find(selement)==services.end()) {
                services[selement] = list<string>();
            }
            services[selement].push_back(snode);
        }
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex->post();
    
    reply.addInt(1);
    reply.addString("current system state from YARP");
    Bottle& bot = reply.addList();
    bot.addString("list");
    addStateGroup(bot.addList(),publishers);
    addStateGroup(bot.addList(),subscribers);
    addStateGroup(bot.addList(),services);
    printf("Reply is %s\n", reply.toString().c_str());
    return true;
}

bool RosAdaptor::getTopicTypes(yarp::os::Bottle& cmd,
                               yarp::os::Bottle& reply) {
    
    mutex->wait();
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    query = sqlite3_mprintf("SELECT node, element, role, typ FROM rosmaster");
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                    NULL);
    ConstString sresult;
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
    }
    
    map<string, string> topics;
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *node = (char *)sqlite3_column_text(statement,0);
        char *element = (char *)sqlite3_column_text(statement,1);
        char *role = (char *)sqlite3_column_text(statement,2);
        char *typ = (char *)sqlite3_column_text(statement,3);
        if (node==NULL||element==NULL||role==NULL||typ==NULL) continue;
        string snode = node;
        string selement = element;
        string srole = role;
        string styp = typ;
        if (srole=="publisher"||srole=="subscriber") {
            if (topics.find(selement)==topics.end()) {
                topics[selement] = "*";
            }
            if (topics[selement]=="*") {
                topics[selement] = styp;
            }
        }
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex->post();
    
    reply.addInt(1);
    reply.addString("current topic types from YARP");
    Bottle& bot = reply.addList();
    bot.addString("list");
    for (map<string,string>::const_iterator it = topics.begin();
         it != topics.end(); it++) {
        Bottle& part = bot.addList();
        part.addString("list");
        part.addString(it->first.c_str());
        part.addString(it->second.c_str());
    }
    printf("Reply is %s\n", reply.toString().c_str());
    
    return true;
}



bool RosAdaptor::lookupNode(yarp::os::Bottle& cmd,
                            yarp::os::Bottle& reply) {
    ConstString node = cmd.get(2).asString();
    
    mutex->wait();
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    query = sqlite3_mprintf("SELECT uri FROM rosmaster WHERE node = %Q",
                            node.c_str());
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                    NULL);
    ConstString sresult;
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
    }
    if (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        sresult = (const char *)sqlite3_column_text(statement,0);
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex->post();
    
    reply.addInt(1);
    reply.addString("node uri");
    reply.addString(sresult);
    return true;
}



bool RosAdaptor::getPublishedTopics(yarp::os::Bottle& cmd,
                                    yarp::os::Bottle& reply) {

    string subgraph = cmd.get(2).asString().c_str();
    subgraph += "%";

    mutex->wait();
    sqlite3_stmt *statement = NULL;
    char *query = NULL;
    query = sqlite3_mprintf("SELECT DISTINCT element, typ FROM rosmaster WHERE role = %Q AND element LIKE %Q", "publisher", subgraph.c_str());
    if (verbose) {
        printf("Query: %s\n", query);
    }
    int result = sqlite3_prepare_v2(SQLDB(implementation),query,-1,&statement,
                                    NULL);
    ConstString sresult;
    if (result!=SQLITE_OK) {
        printf("Error in query\n");
    }

    map<string, string> topics;
    while (result == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        char *element = (char *)sqlite3_column_text(statement,0);
        char *typ = (char *)sqlite3_column_text(statement,1);
        if (element==NULL||typ==NULL) continue;
        string selement = element;
        string styp = typ;
        if (topics.find(selement)==topics.end()) {
            topics[selement] = "*";
        }
        if (topics[selement]=="*") {
            topics[selement] = styp;
        }
    }
    sqlite3_finalize(statement);
    sqlite3_free(query);
    mutex->post();

    reply.addInt(1);
    reply.addString("current published topic types from YARP");
    Bottle& bot = reply.addList();
    bot.addString("list");
    for (map<string,string>::const_iterator it = topics.begin();
         it != topics.end(); it++) {
        Bottle& part = bot.addList();
        part.addString("list");
        part.addString(it->first.c_str());
        part.addString(it->second.c_str());
    }
    printf("Reply is %s\n", reply.toString().c_str());
    return true;
}



bool RosAdaptor::registerPublisher(yarp::os::Bottle& cmd,
                                   yarp::os::Bottle& reply) {
    ConstString node = cmd.get(1).asString();
    ConstString topic = cmd.get(2).asString();
    ConstString typ = cmd.get(3).asString();
    ConstString uri = cmd.get(4).asString();
    
    mutex->wait();
    
    char *msg = NULL;
    char *query = sqlite3_mprintf("INSERT OR REPLACE INTO rosmaster (node,element,role,typ,uri,comment) VALUES(%Q,%Q,%Q,%Q,%Q,%Q)", 
                                  node.c_str(),
                                  topic.c_str(),
                                  "publisher",
                                  typ.c_str(),
                                  uri.c_str(),
                                  "");
    if (verbose) {
        printf("Query: %s\n", query);
    }
    bool ok = true;
    int result = sqlite3_exec(SQLDB(implementation), query, 
                              NULL, NULL, &msg);
    if (result!=SQLITE_OK) {
        ok = false;
        if (msg!=NULL) {
            fprintf(stderr,"Error: %s\n", msg);
            sqlite3_free(msg);
        }
    }
    sqlite3_free(query);    
    
    mutex->post();
    
    return true;
}



bool RosAdaptor::apply(yarp::os::Bottle& cmd, 
                       yarp::os::Bottle& reply, 
                       yarp::os::Bottle& event,
                       const yarp::os::Contact& remote) {
    printf("] %s\n", cmd.toString().c_str());
    ConstString tag = cmd.get(0).asString();
    bool ok = false;
    if (tag=="getSystemState") {
        ok = getSystemState(cmd,reply);
    } else if (tag=="getTopicTypes") {
        ok = getTopicTypes(cmd,reply);
    } else if (tag=="lookupNode") {
        ok = lookupNode(cmd,reply);
    } else if (tag=="getPublishedTopics") {
        ok = getPublishedTopics(cmd,reply);
    } else if (tag=="registerPublisher") {
        ok = registerPublisher(cmd,reply);
    }
    return ok;
}

