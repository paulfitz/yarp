// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Author: Daniel Krieg krieg@fias.uni-frankfurt.de
 * Copyright (C) 2010 Daniel Krieg
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP_MPIBCASTCARRIER_
#define _YARP_MPIBCASTCARRIER_

#include <yarp/os/impl/MpiCarrier.h>
#include <yarp/os/impl/MpiBcastStream.h>

#include <yarp/os/impl/Election.h>


namespace yarp {
    namespace os {
        namespace impl {
            class MpiBcastCarrier;
        }
    }
}

/**
 * Carrier for port communicating via MPI broadcast.
 *
 * Effective collective operations in the one-sender-multiple-receiver scenario.
 * Allows only one-way communication (no replies).
 *
 * @warning Probably all processes terminate
 * if one terminates without proper disconnect.
 * @warning Seems to work, but still experimental.
 */
class yarp::os::impl::MpiBcastCarrier : public MpiCarrier {
private:
    static yarp::os::impl::ElectionOf<MpiBcastCarrier,yarp::os::impl::PeerRecord>* caster;
    static yarp::os::impl::ElectionOf<MpiBcastCarrier,yarp::os::impl::PeerRecord>& getCaster();

    bool electionMember;

public:
    MpiBcastCarrier() : MpiCarrier(), electionMember(false) {
        target = "MPIBCAST";
    }
    ~MpiBcastCarrier();
    void close();
    Carrier *create() {
        return new MpiBcastCarrier();
    }
    void createStream(bool sender);
    String getName() {
        return "bcast";}
    bool supportReply() {
        return false;}
    bool isBroadcast() {
        return true;
    }
    void prepareDisconnect();

    bool expectReplyToHeader(Protocol&  proto) {
        bool ok = MpiCarrier::expectReplyToHeader(proto);
        dynamic_cast<MpiBcastStream*> (stream)->post();
        return ok;
    }

    bool isActive();
    bool isElect();

};

#endif //_YARP_MPIBCASTCARRIER_


