// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <string>
#include <ctype.h>

#include "AllocatorOnTriples.h"

using namespace yarp::os;
using namespace std;

struct YarpElement {
    int count;
    string name;
    string code;
};

static YarpElement elements[] = {
    {1,"Hydrogen","H"},
    {2,"Helium","He"},
    {3,"Lithium","Li"},
    {4,"Beryllium","Be"},
    {5,"Boron","B"},
    {6,"Carbon","C"},
    {7,"Nitrogen","N"},
    {8,"Oxygen","O"},
    {9,"Fluorine","F"},
    {10,"Neon","Ne"},
    {11,"Sodium","Na"},
    {12,"Magnesium","Mg"},
    {13,"Aluminium","Al"},
    {14,"Silicon","Si"},
    {15,"Phosphorus","P"},
    {16,"Sulphur","S"},
    {17,"Chlorine","Cl"},
    {18,"Argon","Ar"},
    {19,"Potassium","K"},
    {20,"Calcium","Ca"},
    {21,"Scandium","Sc"},
    {22,"Titanium","Ti"},
    {23,"Vanadium","V"},
    {24,"Chromium","Cr"},
    {25,"Manganese","Mn"},
    {26,"Iron","Fe"},
    {27,"Cobalt","Co"},
    {28,"Nickel","Ni"},
    {29,"Copper","Cu"},
    {30,"Zinc","Zn"},
    {31,"Gallium","Ga"},
    {32,"Germanium","Ge"},
    {33,"Arsenic","As"},
    {34,"Selenium","Se"},
    {35,"Bromine","Br"},
    {36,"Krypton","Kr"},
    {37,"Rubidium","Rb"},
    {38,"Strontium","Sr"},
    {39,"Yttrium","Y"},
    {40,"Zirkonium","Zr"},
    {41,"Niobium","Nb"},
    {42,"Molybdaenum","Mo"},
    {43,"Technetium","Tc"},
    {44,"Ruthenium","Ru"},
    {45,"Rhodium","Rh"},
    {46,"Palladium","Pd"},
    {47,"Silver","Ag"},
    {48,"Cadmium","Cd"},
    {49,"Indium","In"},
    {50,"Tin","Sn"},
    {51,"Antimony","Sb"},
    {52,"Tellurium","Te"},
    {53,"Iodine","I"},
    {54,"Xenon","Xe"},
    {55,"Cesium","Cs"},
    {56,"Barium","Ba"},
    {57,"Lanthanum","La"},
    {58,"Cerium","Ce"},
    {59,"Praseodymium","Pr"},
    {60,"Neodymium","Nd"},
    {61,"Promethium","Pm"},
    {62,"Samarium","Sm"},
    {63,"Europium","Eu"},
    {64,"Gadolinium","Gd"},
    {65,"Terbium","Tb"},
    {66,"Dysprosium","Dy"},
    {67,"Holmium","Ho"},
    {68,"Erbium","Er"},
    {69,"Thulium","Tm"},
    {70,"Ytterbium","Yb"},
    {71,"Lutetium","Lu"},
    {72,"Hafnium","Hf"},
    {73,"Tantalum","Ta"},
    {74,"Tungsten","W"},
    {75,"Rhenium","Re"},
    {76,"Osmium","Os"},
    {77,"Iridium","Ir"},
    {78,"Platinum","Pt"},
    {79,"Gold","Au"},
    {80,"Hydrargyrum","Hg"},
    {81,"Thallium","Tl"},
    {82,"Lead","Pb"},
    {83,"Bismuth","Bi"},
    {84,"Polonium","Po"},
    {85,"Astatine","At"},
    {86,"Radon","Rn"},
    {87,"Francium","Fr"},
    {88,"Radium","Ra"},
    {89,"Actinium","Ac"},
    {90,"Thorium","Th"},
    {91,"Protactinium","Pa"},
    {92,"Uranium","U"},
    {93,"Neptunium","Np"},
    {94,"Plutonium","Pu"},
    {95,"Americium","Am"},
    {96,"Curium","Cm"},
    {97,"Berkelium","Bk"},
    {98,"Californium","Cf"},
    {99,"Einsteinium","Es"},
    {100,"Fermium","Fm"},
    {101,"Mendelevium","Md"},
    {102,"Nobelium","No"},
    {103,"Lawrencium","Lr"},
    {104,"Rutherfordium","Rf"},
    {105,"Dubnium","Db"},
    {106,"Seaborgium","Sg"},
    {107,"Bohrium","Bh"},
    {108,"Hassium","Hs"},
    {109,"Meitnerium","Mt"},
    {110,"Ununnilium","Ds"},
    {111,"Unununium","Rg"},
    {112,"Ununbium","Uub"},
    {113,"Ununtrium","Uut"},
    {114,"Ununquadium","Uuq"},
    {115,"Ununpentium","Uup"},
    {116,"Ununhexium","Uuh"},
    {118,"Ununoctium","Uuo"}
};

#define YARP_TOP_ELEMENT 118

Contact AllocatorOnTriples::completePortName(const Contact& c) {
    string name = "";
    Triple t;
    t.setNsNameValue("alloc","tmpid","*");
    TripleContext context;
    context.setRid(db->find(t,NULL));
    if (context.rid>=0) {
        t.setNsNameValue("alloc","*","free");
        list<Triple> match = db->query(t,&context);
        if (match.size()>0) {
            name = match.begin()->name;
        }
    }

    if (name=="") {
        if (tmpid==-1) {
            t.setNsNameValue("alloc","tmpid","*");
            list<Triple> lst = db->query(t,NULL);
            if (lst.size()>0) {
                tmpid = atoi(lst.begin()->value.c_str());
            } 
            if (tmpid==-1) {
                tmpid = 0;
            }
        }
        tmpid++;
        char buf[256];
        sprintf(buf,"%d",tmpid);
        t.setNsNameValue("alloc","tmpid",buf);
        db->update(t,NULL);
        t.setNsNameValue("alloc","tmpid","*");
        context.setRid(db->find(t,NULL));
        /*
          // not ready to switch /tmp port names yet
        if (tmpid>=1 && tmpid<=YARP_TOP_ELEMENT) {
            string name = elements[tmpid-1].code;
            std::transform(name.begin(),name.end(),name.begin(),::tolower);
            sprintf(buf,"/atom/%s", name.c_str());
        } else {
            sprintf(buf,"/atom/%u", tmpid);
        }
        */
        sprintf(buf,"/tmp/port/%u", tmpid);
        name = buf;
    }

    t.setNsNameValue("alloc",name.c_str(),"in_use");
    db->update(t,&context);
    
    return Contact::byName(name.c_str()).addSocket(c.getCarrier().c_str(),
                                                   c.getHost().c_str(),
                                                   c.getPort());
}


Contact AllocatorOnTriples::completeSocket(const Contact& c) {
    Contact tmp = completeHost(c);
    return completePortNumber(tmp);
}

Contact AllocatorOnTriples::completePortNumber(const Contact& c) {
    if (c.getPort()!=-1 && c.getPort()!=0) {
        return c;
    }

    // unlike standard yarp name server, port number allocation
    // is global across the network, rather than per machine.

    // we also try to keep port numbers stable for port names,
    // when possible.

    string npref = "";
    int pref = -1;
    string nstring = "";
    int number = -1;
    Triple t;
    t.setNsNameValue("alloc","regid","*");
    TripleContext context;
    context.setRid(db->find(t,NULL));
    if (context.rid>=0) {
        t.setNsNameValue("prefer","*",c.getName().c_str());
        list<Triple> match = db->query(t,&context);
        if (match.size()>0) {
            npref = match.begin()->name;
            pref = atoi(npref.c_str());
            t.setNsNameValue("alloc",npref.c_str(),"in_use");
            match = db->query(t,&context);
            if (match.size()==0) {
                nstring = npref;
                number = pref;
            }
        }
    }

    if (nstring=="") {
        if (regid==-1) {
            Triple t;
            t.setNsNameValue("alloc","regid","*");
            list<Triple> lst = db->query(t,NULL);
            if (lst.size()>0) {
                regid = atoi(lst.begin()->value.c_str());
            } 
            if (regid==-1) {
                regid = config.minPortNumber-1;
            }
        }
        if (regid>=config.maxPortNumber && config.maxPortNumber!=0) {
            if (nstring == "") {
                t.setNsNameValue("alloc","*","free");
                list<Triple> match = db->query(t,&context);
                if (match.size()>0) {
                    nstring = match.begin()->name;
                    number = atoi(nstring.c_str());
                }
            }
            if (nstring=="") {
                fprintf(stderr,"Ran out of port numbers\n");
                fprintf(stderr,"* Make sure ports/programs get closed properly.\n");
                fprintf(stderr,"* If programs terminate without closing ports, run \"yarp clean\" from time to time..\n");
                exit(1);
            }
        } else {
            regid++;
            Triple t;
            char buf[256];
            sprintf(buf,"%d",regid);
            t.setNsNameValue("alloc","regid",buf);
            db->update(t,NULL);
            t.setNsNameValue("alloc","regid","*");
            context.setRid(db->find(t,NULL));
            nstring = buf;
            number = regid;
        }
    }
    t.setNsNameValue("alloc",nstring.c_str(),"in_use");
    db->update(t,&context);
    t.setNsNameValue("prefer",nstring.c_str(),c.getName().c_str());
    db->update(t,&context);
     
    return c.addSocket(c.getCarrier().c_str(),
                       c.getHost().c_str(),
                       number);
}


Contact AllocatorOnTriples::completeHost(const yarp::os::Contact& c) {
    // deal with allocating multicast ips

    if (c.getCarrier()!="mcast") {
        return c;
    }
    if (c.getHost()!="...") {
        return c;
    }

    string name = "";
    Triple t;
    t.setNsNameValue("alloc","mcastCursor","*");
    TripleContext context;
    context.setRid(db->find(t,NULL));
    if (context.rid>=0) {
        t.setNsNameValue("alloc","*","free");
        list<Triple> match = db->query(t,&context);
        if (match.size()>0) {
            name = match.begin()->name;
        }
    }

    if (name=="") {
        if (mcastCursor==-1) {
            t.setNsNameValue("alloc","mcastCursor","*");
            list<Triple> lst = db->query(t,NULL);
            if (lst.size()>0) {
                mcastCursor = atoi(lst.begin()->value.c_str());
            } 
            if (mcastCursor==-1) {
                mcastCursor = 1;
            }
        }
        mcastCursor++;
        char buf[256];
        sprintf(buf,"%d",mcastCursor);
        t.setNsNameValue("alloc","mcastCursor",buf);
        db->update(t,NULL);
        t.setNsNameValue("alloc","mcastCursor","*");
        context.setRid(db->find(t,NULL));

        int v1 = mcastCursor%255;
        int v2 = mcastCursor/255;
        if (v2>=255) {
            fprintf(stderr,"Ran out of mcast addresses\n");
            exit(1);
        }
        sprintf(buf,"224.1.%d.%d", v2+1,v1+1);
        name = buf;
    }
    
    t.setNsNameValue("alloc",name.c_str(),"in_use");
    db->update(t,&context);
    
    return c.addSocket(c.getCarrier().c_str(),
                       name.c_str(),
                       c.getPort());
}


bool AllocatorOnTriples::freePortResources(const yarp::os::Contact& c) {
    string portName = c.getName().c_str();
    int portNumber = c.getPort();
    string hostName = c.getHost().c_str();

    // free up automatic name for port, if one was allocated
    Triple t;
    t.setNsNameValue("alloc","tmpid","*");
    TripleContext context;
    context.setRid(db->find(t,NULL));
    t.setNsNameValue("alloc",portName.c_str(),"in_use");
    if (db->find(t,&context)>=0) {
        t.setNsNameValue("alloc",portName.c_str(),"free");
        db->update(t,&context);
    }

    t.setNsNameValue("alloc","regid","*");
    context.setRid(db->find(t,NULL));
    char buf[256];
    sprintf(buf,"%d",portNumber);
    t.setNsNameValue("alloc",buf,"in_use");
    if (db->find(t,&context)>=0) {
        t.setNsNameValue("alloc",buf,"free");
        db->update(t,&context);
    }

    t.setNsNameValue("alloc","mcastCursor","*");
    context.setRid(db->find(t,NULL));
    t.setNsNameValue("alloc",hostName.c_str(),"in_use");
    if (db->find(t,&context)>=0) {
        t.setNsNameValue("alloc",hostName.c_str(),"free");
        db->update(t,&context);
    }

    return true;
}



