// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/RosNameSpace.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Nodes.h>
#include <yarp/os/impl/NameClient.h>

using namespace yarp::os;
using namespace yarp::os::impl;

RosNameSpace::RosNameSpace(const Contact& contact) {
    this->contact = contact;
}

RosNameSpace::~RosNameSpace() {
}

Contact RosNameSpace::getNameServerContact() const {
    return contact;
}

Contact RosNameSpace::queryName(const ConstString& name) {
    printf("RosNameSpace::queryName(\"%s\")\n", name.c_str());
    return Contact();

    /*
    NestedContact nc;
    nc.fromString(name);

    ConstString node = nc.getNodeName();
    ConstString topic = nc.getNestedName();
    Bottle cmd,reply;
    cmd.addString("lookupNode");
    cmd.addString("dummy_id");
    cmd.addString(toRosNodeName(node));
    printf("PRE Looking up %s -> %s\n", node.c_str(), reply.toString().c_str());
    NetworkBase::write(getNameServerContact(),
                       cmd, reply);
    printf("Looking up %s -> %s\n", node.c_str(), reply.toString().c_str());
    if (reply.get(0).asInt()!=1) {
        cmd.clear();
        reply.clear();
        cmd.addString("lookupService");
        cmd.addString("dummy_id");
        cmd.addString(toRosNodeName(node));
        NetworkBase::write(getNameServerContact(),
                           cmd, reply);
    }
    contact = Contact::fromString(reply.get(2).asString());
    if (contact.isValid()) {
        contact = contact.addCarrier("xmlrpc");
    }
    printf("  GOT %s\n", contact.toString().c_str());
    if (topic=="") {
        return contact;
    }

    printf("  GOING DEEPER for %s\n", contact.toString().c_str());
    contact = contact.addName("");
    Bottle req;
    reply.clear();
    req.addString("requestTopic");
    req.addString("dummy_id");
    req.addString(topic);
    Bottle& lst = req.addList();
    Bottle& sublst = lst.addList();
    sublst.addString("TCPROS");
    printf("Looking up %s\n", req.toString().c_str());
    if (!NetworkBase::write(contact,req,reply,false,false)) {
        fprintf(stderr,"Failure looking up topic %s: %s\n", topic.c_str(), reply.toString().c_str());
        return Contact();
    }
    printf("Looking up %s -> %s\n", req.toString().c_str(), reply.toString().c_str());
    Bottle *pref = reply.get(2).asList();
    if (pref==NULL) {
        fprintf(stderr,"Failure looking up service %s: expected list of protocols\n", topic.c_str());
        return Contact();
    }
    if (pref->get(0).asString()!="TCPROS") {
        if (pref->get(0).asString() == "faultString") {
            fprintf(stderr,"Failure looking up topic %s: %s\n", topic.c_str(),
                    pref->toString().c_str());
        } else {
            fprintf(stderr,"Failure looking up topic %s: unsupported protocol %s\n", topic.c_str(),
                    pref->get(0).asString().c_str());
        }
        return Contact();
    }
    Value hostname2 = pref->get(1);
    Value portnum2 = pref->get(2);
    contact = contact.addSocket(ConstString("tcpros+role.pub+topic.") + topic,
                                hostname2.asString().c_str(),
                                portnum2.asInt());

    return contact;
    */
}

Contact RosNameSpace::registerName(const ConstString& name) {
    fprintf(stderr,"ROS name server does not do 'raw' registrations.\n");
    fprintf(stderr,"Use [Buffered]Port::open to get complete registrations.\n");
    exit(1);

    return Contact();
}

Contact RosNameSpace::registerContact(const Contact& contact) {
    NestedContact nc;
    nc.fromString(contact.getName());
    //printf("CONTACT %s category %s\n", nc.getFullName().c_str(),
    //nc.getCategory().c_str());
    ConstString cat = nc.getCategory();
    if (nc.getNestedName()!="") {
        if (cat == "+" || cat== "-") {
            Bottle cmd, reply;
            cmd.clear();
            cmd.addString((cat=="+")?"registerPublisher":"registerSubscriber");
            cmd.addString(toRosNodeName(nc.getNodeName()));
            cmd.addString(nc.getNestedName());
            cmd.addString(nc.getTypeNameStar());
            Nodes& nodes = NameClient::getNameClient().getNodes();
            Contact c = rosify(nodes.getParent(contact.getName()));
            //Contact c = rosify(contact);
            cmd.addString(c.toString());
            bool ok = NetworkBase::write(getNameServerContact(),
                                         cmd, reply);
            printf("Wrote [%s] got [%s]\n", cmd.toString().c_str(),
                   reply.toString().c_str());
            if (!ok) return Contact();
        }
        
        return contact;
    }
    return contact;
}

Contact RosNameSpace::updateContact(const Contact& contact) {
    return Contact();
}


Contact RosNameSpace::unregisterName(const ConstString& rname) {
    NestedContact nc;
    nc.fromString(rname);
    printf("UNREGISTER %s category %s\n", nc.getFullName().c_str(),
           nc.getCategory().c_str());
    ConstString cat = nc.getCategory();

    if (nc.getNestedName()!="") {
        if (cat == "+" || cat== "-") {
            Bottle cmd, reply;
            cmd.clear();
            cmd.addString((cat=="+")?"unregisterPublisher":"unregisterSubscriber");
            cmd.addString(toRosNodeName(nc.getNodeName()));
            cmd.addString(nc.getNestedName());
            Nodes& nodes = NameClient::getNameClient().getNodes();
            Contact c = rosify(nodes.getParent(rname));
            cmd.addString(c.toString());
            bool ok = NetworkBase::write(getNameServerContact(),
                                         cmd, reply);
            printf("Wrote [%s] got [%s]\n", cmd.toString().c_str(),
                   reply.toString().c_str());
            if (!ok) return Contact();
        }
    }

    return Contact();
}

Contact RosNameSpace::unregisterContact(const Contact& contact) {
    return Contact();
}

bool RosNameSpace::setProperty(const ConstString& name,
                               const ConstString& key,
                               const Value& value) {
    return false;
}

Value *RosNameSpace::getProperty(const ConstString& name, 
                                 const ConstString& key) {
        return NULL;
}

bool RosNameSpace::connectPortToTopic(const Contact& src,
                                      const Contact& dest,
                                      ContactStyle style) {
    Bottle cmd;
    cmd.addString("registerPublisher");
    cmd.addString(toRosNodeName(src.getName()));
    cmd.addString(dest.getName());
    cmd.addString("*");
    cmd.addString(rosify(src).toString());

    return connectTopic(cmd,false,src,dest,style,false);
}

bool RosNameSpace::connectTopicToPort(const Contact& src,
                                      const Contact& dest,
                                      ContactStyle style) {
    Bottle cmd;
    cmd.addString("registerSubscriber");
    cmd.addString(toRosNodeName(dest.getName()));
    cmd.addString(src.getName());
    cmd.addString("*");
    cmd.addString(rosify(dest).toString());

    return connectTopic(cmd,true,src,dest,style,true);
}

bool RosNameSpace::disconnectPortFromTopic(const Contact& src,
                                           const Contact& dest,
                                           ContactStyle style) {
    Bottle cmd;
    cmd.addString("unregisterPublisher");
    cmd.addString(toRosNodeName(src.getName()));
    cmd.addString(dest.getName());
    cmd.addString(rosify(src).toString());
    return connectTopic(cmd,false,src,dest,style,false);
}

bool RosNameSpace::disconnectTopicFromPort(const Contact& src,
                                           const Contact& dest,
                                           ContactStyle style) {
    Bottle cmd;
    cmd.addString("unregisterSubscriber");
    cmd.addString(toRosNodeName(dest.getName()));
    cmd.addString(src.getName());
    cmd.addString(rosify(dest).toString());
    return connectTopic(cmd,true,src,dest,style,false);
}

bool RosNameSpace::connectPortToPortPersistently(const Contact& src,
                                                 const Contact& dest,
                                                 ContactStyle style) {
    return false;
}

bool RosNameSpace::disconnectPortToPortPersistently(const Contact& src,
                                                    const Contact& dest,
                                                    ContactStyle style) {
    return false;
}

bool RosNameSpace::connectTopic(Bottle& cmd,
                                bool srcIsTopic,
                                const Contact& src,
                                const Contact& dest,
                                ContactStyle style,
                                bool activeRegistration) {
    Bottle reply;
    Contact dynamicSrc = src;
    Contact dynamicDest = dest;
    if (style.carrier!="") {
        if (srcIsTopic) {
            dynamicDest = dynamicDest.addCarrier(style.carrier);
        } else {
            dynamicSrc = dynamicSrc.addCarrier(style.carrier);
        }
    }
    Contact base = getNameServerContact();
    bool ok = NetworkBase::write(base,
                                    cmd,
                                    reply);
    bool fail = (reply.check("faultCode",Value(0)).asInt()!=0)||!ok;
    if (fail) {
        if (!style.quiet) {
            fprintf(stderr,"Failure: name server did not accept connection to topic.\n");
            if (reply.check("faultString")) {
                fprintf(stderr,"Cause: %s\n", reply.check("faultString",Value("")).asString().c_str());
            }
        }
    }
    if (!fail) {
        if (activeRegistration) {
            Bottle *lst = reply.get(2).asList();
            Bottle cmd2;
            if (lst!=NULL) {
                cmd2.addString("publisherUpdate");
                cmd2.addString("/yarp");
                cmd2.addString(dynamicSrc.getName());
                cmd2.addList() = *lst;
                //printf("ACTIVE REGISTRATION: %s\n", cmd2.toString().c_str());
                NetworkBase::write(dynamicDest,
                                    cmd2,
                                    reply,
                                    true);
            }
        }
    }
    return !fail;
}

bool RosNameSpace::localOnly() const {
    return false;
}

bool RosNameSpace::usesCentralServer() const {
    return true;
}

bool RosNameSpace::serverAllocatesPortNumbers() const {
    return false;
}

bool RosNameSpace::connectionHasNameOfEndpoints() const {
    return false;
}

Contact RosNameSpace::detectNameServer(bool useDetectedServer,
                                       bool& scanNeeded,
                                       bool& serverUsed) {
    NameConfig nc;
    nc.fromFile();
    Contact c = nc.getAddress();
    scanNeeded = false;
    serverUsed = false;

    if (!c.isValid()) {
        scanNeeded = true;
        fprintf(stderr,"Checking for ROS_MASTER_URI...\n");
        ConstString addr = NetworkBase::getEnvironment("ROS_MASTER_URI");
        c = Contact::fromString(addr.c_str());
        if (c.isValid()) {
            c = c.addCarrier("xmlrpc");
            c = c.addName(nc.getNamespace().c_str());
            NameConfig nc;
            nc.setAddress(c);
            nc.setMode("ros");
            nc.toFile();
            serverUsed = true;
         }
    }
    return c;
}

bool RosNameSpace::writeToNameServer(PortWriter& cmd,
                                     PortReader& reply,
                                     const ContactStyle& style) {
    DummyConnector con0;
    cmd.write(con0.getWriter());
    Bottle in;
    in.read(con0.getReader());
    ConstString key = in.get(0).asString();
    ConstString arg1 = in.get(1).asString();

    Bottle cmd2, cache;
    bool use_cache = false;
    if (key=="query") {
        Contact c = queryName(arg1.c_str()).addName("");
        Bottle reply2;
        reply2.addString(arg1.c_str());
        reply2.addString(c.toString().c_str());
        DummyConnector con;
        reply2.write(con.getWriter());
        reply.read(con.getReader());
        return true;
    } else if (key=="list") {
        cmd2.addString("getSystemState");
        cmd2.addString("dummy_id");
        use_cache = true;
    } else {
        return false;
    }
    bool ok = NetworkBase::write(getNameServerContact(),
                                 cmd2,
                                 *(use_cache?&cache:&reply),
                                 style);
    if (!ok) {
        fprintf(stderr,"Failed to contact ROS server\n");
        return false;
    }

    if (key=="list") {
        Bottle out;
        out.addVocab(Vocab::encode("many"));
        Bottle *parts = cache.get(2).asList();
        Property nodes;
        Property topics;
        Property services;
        if (parts) {
            for (int i=0; i<3; i++) {
                Bottle *part = parts->get(i).asList();
                if (!part) continue;
                for (int j=0; j<part->size(); j++) {
                    Bottle *unit = part->get(j).asList();
                    if (!unit) continue;
                    ConstString stem = unit->get(0).asString();
                    Bottle *links = unit->get(1).asList();
                    if (!links) continue;
                    if (i<2) {
                        topics.put(stem,1);
                    } else {
                        services.put(stem,1);
                    }
                    for (int j=0; j<links->size(); j++) {
                        nodes.put(links->get(j).asString(),1);
                    }
                }
            }
            Property *props[3] = {&nodes, &topics, &services};
            const char *title[3] = {"node", "topic", "service"};
            for (int p=0; p<3; p++) {
                Bottle blist;
                blist.read(*props[p]);
                for (int i=0; i<blist.size(); i++) {
                    ConstString name = blist.get(i).asList()->get(0).asString();
                    Bottle& info = out.addList();
                    info.addString(title[p]);
                    info.addString(name);
                }
            }
        }
        //if (parts) out.append(*parts);
        out.write(reply);
    }


    return ok;

}


ConstString RosNameSpace::toRosName(const ConstString& name) {
    if (name.find(":")==ConstString::npos) return name;
    ConstString result;
    for (size_t i=0; i<name.length(); i++) {
        if (name[i]!=':') {
            result += name[i];
        } else {
            result += "__";
        }
    }
    return result;
}

ConstString RosNameSpace::fromRosName(const ConstString& name) {
    if (name.find("__")==ConstString::npos) return name;
    // length is at least 2
    ConstString result;
    int ct = 0;
    for (size_t i=0; i<name.length(); i++) {
        if (name[i]!='_') {
            if (ct) result += '_';
            result += name[i];
            ct = 0;
        } else {
            ct++;
            if (ct==2) {
                result += ':';
                ct = 0;
            }
        }
    }
    if (ct) result += '_';
    return result;
}

ConstString RosNameSpace::toRosNodeName(const ConstString& name) {
    return toRosName(name);
}

ConstString RosNameSpace::fromRosNodeName(const ConstString& name) {
    return fromRosName(name);
}

Contact RosNameSpace::rosify(const Contact& contact) {
    return Contact::bySocket("http",contact.getHost().c_str(),
                             contact.getPort());
}
