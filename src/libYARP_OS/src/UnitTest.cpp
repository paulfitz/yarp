// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/Logger.h>

#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/Network.h>
#include <math.h>

using namespace yarp::os::impl;
using namespace yarp::os;

UnitTest *UnitTest::theRoot = NULL;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class RootUnitTest : public UnitTest {
public:
    RootUnitTest(UnitTest *parent) : UnitTest(parent) {
        // no parent
    }

    virtual String getName() {
        return "root";
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/



UnitTest::UnitTest() {
    parent = &UnitTest::getRoot();
    hasProblem = false;
}

UnitTest::UnitTest(UnitTest *parent) {
    this->parent = parent;
    if (parent!=NULL) {
        parent->add(*this);
    }
    hasProblem = false;
}

void UnitTest::add(UnitTest& unit) {
    for (unsigned int i=0; i<subTests.size(); i++) {
        if (subTests[i]==&unit) {
            return; // already present, no need to add
        }
    }
    subTests.push_back(&unit);
}

void UnitTest::clear() {
    subTests.clear();
}

void UnitTest::report(int severity, const String& problem) {
    if (parent!=NULL) {
        parent->report(severity, getName() + ": " + problem);
    } else {
        YARP_SPRINTF2(Logger::get(),info,
                      "%d | %s", severity, problem.c_str());
    }
    count(severity);
}

void UnitTest::count(int severity) {
    if (severity>0) {
        // could do something more sophisticated with the reports than this...
        hasProblem = true;
    }
}


void UnitTest::runSubTests(int argc, char *argv[]) {
    for (unsigned int i=0; i<subTests.size(); i++) {
        subTests[i]->run(argc,argv);
    }
}


int UnitTest::run() {
    run(0,NULL);
    return hasProblem;
}


int UnitTest::run(int argc, char *argv[]) {
    bool ran = false;
    if (argc==0) {
        runTests();
        ran = true;
    } else {
        String name = getName();
        bool onList = false;
        for (int i=0; i<argc; i++) {
            if (name == String(argv[i])) {
                onList = true;
                break;
            }
        }
        if (onList) {
            runTests();
            ran = true;
        }
    }
    runSubTests(argc,argv);
    if (hasProblem) {
        report(0,"A PROBLEM WAS ENCOUNTERED");
    } 
    else {
        if (ran) {
            report(0,"no problems reported");
        }
    }
    return hasProblem;
}



void UnitTest::startTestSystem() {
    if (theRoot==NULL) {
        theRoot = new RootUnitTest(NULL);
    }
}

// system starts on first call, probably from a static object - this
// is to avoid link order dependency problems
UnitTest& UnitTest::getRoot() {
    startTestSystem();
    yAssert(theRoot!=NULL);
    return *theRoot;
}

// this is the important one to call
void UnitTest::stopTestSystem() {
    if (theRoot!=NULL) {
        delete theRoot;
        theRoot = NULL;
    }
}


bool UnitTest::checkEqualImpl(int x, int y, 
                              const char *desc,
                              const char *txt1,
                              const char *txt2,
                              const char *fname,
                              int fline) {
    char buf[1000];
    ACE_OS::sprintf(buf, "in file %s:%d [%s] %s (%d) == %s (%d)",
                    fname, fline, desc, txt1, x, txt2, y);
    if (x==y) {
        report(0,String("  [") + desc + "] passed ok");
    } else {
        report(1,String("  FAILURE ") + buf);
    }
    return x==y;
}

bool UnitTest::checkEqualishImpl(double x, double y, 
                                 const char *desc,
                                 const char *txt1,
                                 const char *txt2,
                                 const char *fname,
                                 int fline) {
    char buf[1000];
    ACE_OS::sprintf(buf, "in file %s:%d [%s] %s (%g) == %s (%g)",
                    fname, fline, desc, txt1, x, txt2, y);
    bool ok = (fabs(x-y)<0.0001);
    if (ok) {
        report(0,String("  [") + desc + "] passed ok");
    } else {
        report(1,String("  FAILURE ") + buf);
    }
    return ok;
}


bool UnitTest::checkEqualImpl(const String& x, const String& y,
                              const char *desc,
                              const char *txt1,
                              const char *txt2,
                              const char *fname,
                              int fline) {
    char buf[20000];
    ConstString humanized_x = humanize(x);
    ConstString humanized_y = humanize(y);
    ACE_OS::sprintf(buf, "in file %s:%d [%s] %s (%s) == %s (%s)",
                    fname, fline, desc, txt1, humanized_x.c_str(), txt2, humanized_y.c_str());
    bool ok = (x==y);
    if (ok) {
        report(0,String("  [") + desc + "] passed ok");
    } else {
        report(1,String("  FAILURE ") + buf);
    }
    return ok;
}


String UnitTest::humanize(const String& txt) {
    String result("");
    for (unsigned int i=0; i<txt.length(); i++) {
        char ch = txt[i];
        if (ch == '\n') {
            result += "\\n";
        } else if (ch == '\r') {
            result += "\\r";
        } else if (ch == '\0') {
            result += "\\0";
        } else {
            result += ch;
        }
    }
    return result;
}


void UnitTest::saveEnvironment(const char *key) {
    bool found = false;
    ConstString val = NetworkBase::getEnvironment(key,&found);
    Bottle& lst = env.addList();
    lst.addString(key);
    lst.addString(val);
    lst.addInt(found?1:0);
}

void UnitTest::restoreEnvironment() {
    for (int i=0; i<env.size(); i++) {
        Bottle *lst = env.get(i).asList();
        if (lst==NULL) continue;
        ConstString key = lst->get(0).asString();
        ConstString val = lst->get(1).asString();
        bool found = lst->get(2).asInt()?true:false;
        if (!found) {
            NetworkBase::unsetEnvironment(key);
        } else {
            NetworkBase::setEnvironment(key,val);
        }
    }
    env.clear();
}

static int _count_mem = 0;
static bool _counting = false;
static bool _count_block = false;
#include <yarp/os/Mutex.h>
static yarp::os::Mutex *_count_mutex = NULL;
#ifdef YARP_TEST_HEAP
static int _count_new = 0;
static int _count_del = 0;
#include <cstdio>
#include <cstdlib>
void* operator new(std::size_t sz) {
    if (_counting) {
        _count_mutex->lock();
        if (_count_block) {
            fprintf(stderr,"unexpected new detected\n");
            yarp_print_trace(stderr,__FILE__,__LINE__);
        }
        //std::printf("global op new called, size = %zu\n",sz);
        _count_mem++;
        _count_new++;
        _count_mutex->unlock();
    }
    return std::malloc(sz);
}
void operator delete(void* ptr) 
{
    if (_counting) {
        _count_mutex->lock();
        if (_count_block) {
            fprintf(stderr,"unexpected delete detected\n");
            yarp_print_trace(stderr,__FILE__,__LINE__);
        }
        //std::puts("global op delete called");
        _count_mem++;
        _count_del++;
        _count_mutex->unlock();
    }
    std::free(ptr);
}
#endif


bool UnitTest::heapMonitorSupported() {
#ifdef YARP_TEST_HEAP
    return true;
#else
    return false;
#endif
}

void UnitTest::heapMonitorBegin() {
    // please, do not be allocating yet, no threads
    heapMonitorEnd();
    _count_mutex = new Mutex();
    _count_mem = 0;
    _counting = true;
}

void UnitTest::heapMonitorBlock() {
    heapMonitorEnd();
    _count_block = true;
    _count_mutex = new Mutex();
    _count_mem = 0;
    _counting = true;
}

int UnitTest::heapMonitorOps() {
    _count_mutex->lock();
    int del = _count_mem;
    _count_mem = 0;
    _count_mutex->unlock();
    return del;
}


int UnitTest::heapMonitorEnd() {
    // please, be done allocating, no threads
    _counting = false;
    if (_count_block) {
        checkEqual(0,_count_mem,"heap operator count");
    }
    _count_block = false;
    if (_count_mutex==NULL) return 0;
    delete _count_mutex;
    _count_mutex = NULL;
    return _count_mem;
}


