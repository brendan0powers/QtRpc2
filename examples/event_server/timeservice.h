/***************************************************************************
 *  Copyright (c) 2010, Resara LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Resara LLC nor the
 *       names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL RESARA LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************/
#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include <ServiceProxy>
#include <QTimer>
#include <QDateTime>

using namespace QtRpc;

//All services must inherit from the ServiceProxy class.
//Inheriting from another service is also supported.
class TimeService : public ServiceProxy
{
    Q_OBJECT
public:
    explicit TimeService(QObject *parent = 0);

    //This is the authentication function called by QtRpc2 uppon connection
    virtual ReturnValue auth(QString user, QString pass);
    
    //To allow functions to be called over the network, they must be public
    //slots, and have a return type of ReturnValue. This automatically registers
    //them to be used for a QtRpc2 service.
public slots:
    //This function does not use ReturnValue as a return type, and is not callable
    //from a client.
    void timeout();

    //Signals with a return type of Event are events. And act like
    //normal Qt signals. You can connect to and emit them as normal
    //The events will be transfered over the network and re-emitted
    //on the client side.
signals:
    //This event get's emitted every 1 second
    Event currentTime(QDateTime time);
private:
    QTimer *m_timer;

};

#endif // TIMESERVICE_H
