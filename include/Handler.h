// Copyright (c) 2014 The SealedServer Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include "Socket.h"

namespace sealedserver
{

class EventLoop;

/**
** A handler that receives notifications on events on a file descriptor.
** The subclass may call registerxxxx/unRegisterxxx to enable/disable 
** read/write events
** Handlers are only active when attached to both an event loop
** and callback(io schedule)
 */

class Handler
{
public:
    /// Constructor.
    Handler(EventLoop* loop);

    /// Destructor
    virtual ~Handler();

public:
    /// Register Read/Write Events
    void registerRead();
    void registerWrite();

    // Unregister Read/Write Events
    void unRegisterRead();
    void unRegisterWrite();


public:
    /// Called by the event loop when bytes are available on the file
    /// descriptor (if read event happens).
    virtual void onReceiveMsg()  = 0;

    /// Called by the event loop when there are bytes need to be written
    /// and written is enable
    virtual void onSendMsg()     = 0;

    /// Called by the event loop when there close events happens
    virtual void onCloseSocket(int st) = 0;

public:
    /// Helper functions

    /// Return the socket
    Socket getSocket() const;

    /// Return the loop it registers with
    EventLoop *getLoop() const;

    /// Return the status
    int getStatus() const;

    /// Get the delete flag;
    int getDelflag() const;

    /// Set the delete flag
    /// Later it will be removed from memory
    int setDelflag();

    /// Update the registered event status
    ///
    /// @param val, the event need to be registered with
    void updateStatus(int val);

    /// Remove the status `val`
    ///
    /// @param val, the event need to be unregistered with
    void removeStatus(int val);

    /// Get random loop
    EventLoop* getRandomLoop();
protected:
    /// Attach `this` to loop
    void attach();

    /// Detach `this` from loop
    void detach();

protected:
    
    EventLoop*  m_loop;
    Socket      m_sock;

    /// Socket status
    int         m_status;
    
    /// Delete flag
    int         m_delflag;
};

};

#endif