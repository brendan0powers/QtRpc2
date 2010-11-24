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
#include "message.h"
#include "message_p.h"
#include <QDebug>

using namespace QtRpc;

quint32 Message::currentVersion()
{
	return 0x00000002;
}

MessageData::MessageData()
{
	id = 0;
	type = Message::Invalid;
	version = 0x00000000; // original qtrpc2
	service = 0;
}

/**
 * Constructor
 */
Message::Message()
{
	qxt_d().data = new MessageData();
}

/**
 * The copy constructor to build a new Message from an old one.
 * @param msg Message to copy from
 */
Message::Message(const Message& other)
{
	qxt_d().data = other.qxt_d().data;
}

/**
 * Constructor to build a message from a set of variables
 * @param id The id number of the message
 * @param type The type of the message
 * @param func The Signature object of the message
 * @param args The Arguments list for the message, or the components of the ReturnValue
 */
Message::Message(uint id, Type type, Signature func, Arguments args)
{
	qxt_d().data = new MessageData();
	qxt_d().data->id = id;
	qxt_d().data->type = type;
	qxt_d().data->func = func;
	qxt_d().data->args = args;
	if (qxt_d().data->type == Return)
	{
		if (qxt_d().data->args.count() > 1)
		{
			qxt_d().data->ret = ReturnValue(qxt_d().data->args[0].toInt(), qxt_d().data->args[1].toString());
		}
		else if (qxt_d().data->args.count() == 1)
		{
			qxt_d().data->ret = ReturnValue(qxt_d().data->args[0]);
		}
		else
		{
			qxt_d().data->ret = ReturnValue(100, "Error: Failed to generate the ReturnValue");
		}
	}
}

Message::Message(uint id, Type type, Signature func, Arguments args, quint32 service)
{
	qxt_d().data = new MessageData();
	qxt_d().data->id = id;
	qxt_d().data->type = type;
	qxt_d().data->func = func;
	qxt_d().data->args = args;
	qxt_d().data->service = service;
	if (qxt_d().data->type == Return)
	{
		if (qxt_d().data->args.count() > 1)
		{
			qxt_d().data->ret = ReturnValue(qxt_d().data->args[0].toInt(), qxt_d().data->args[1].toString());
		}
		else if (qxt_d().data->args.count() == 1)
		{
			qxt_d().data->ret = ReturnValue(qxt_d().data->args[0]);
		}
		else
		{
			qxt_d().data->ret = ReturnValue(100, "Error: Failed to generate the ReturnValue");
		}
	}
}

Message::Message(uint id, ReturnValue ret)
{
	qxt_d().data = new MessageData();
	qxt_d().data->id = id;
	qxt_d().data->type = Return;
	setReturnValue(ret);
	qxt_d().data->service = 0;
}

Message Message::operator=(const Message & other)
{
	qxt_d().data = other.qxt_d().data;
	return *this;
}

quint32 Message::version() const
{
	QReadLocker lock(qxt_d().constMutex());
	return qxt_d().data->version;
}

void Message::setVersion(quint32 version)
{
	QWriteLocker lock(qxt_d().constMutex());
	qxt_d().data->version = version;
}

/**
 * This function gets the id of the message
 * @return The id of the message
 */
uint Message::id() const
{
	QReadLocker lock(qxt_d().constMutex());
	return qxt_d().data->id;
}

/**
 * Set the id of the message
 * @param id The new id number
 */
void Message::setId(uint id)
{
	QWriteLocker lock(qxt_d().constMutex());
	qxt_d().data->id = id;
}

/**
 * Get the type of message
 * @return The type of the message
 */
Message::Type Message::type() const
{
	QReadLocker lock(qxt_d().constMutex());
	return qxt_d().data->type;
}

/**
 * Set the type of message
 * @param type The new type
 */
void Message::setType(Type type)
{
	QWriteLocker lock(qxt_d().constMutex());
	qxt_d().data->type = type;
}

/**
 * Get the signature of the message
 * @return The Signature of the message
 */
Signature Message::signature() const
{
	QReadLocker lock(qxt_d().constMutex());
	return qxt_d().data->func;
}

/**
 * Set the Signature of the message
 * @param func The new Signature
 */
void Message::setSignature(const Signature& func)
{
	QWriteLocker lock(qxt_d().constMutex());
	qxt_d().data->func = func;
}

/**
 * Get the Arguments of the message
 * @return The Arguments of the message
 */
Arguments Message::arguments() const
{
	QReadLocker lock(qxt_d().constMutex());
	return qxt_d().data->args;
}

/**
 * Set the Arguments of the message. If Type is Return, it will attempt to contruct a ReturnValue from the arguments.
 * @param args The new Arguments
 */
void Message::setArguments(const Arguments& args)
{
	QWriteLocker lock(qxt_d().constMutex());
	qxt_d().data->args = args;
	if (qxt_d().data->type == Return)
	{
		if (qxt_d().data->args.count() > 1)
		{
			qxt_d().data->ret = ReturnValue(qxt_d().data->args[0].toInt(), qxt_d().data->args[1].toString());
		}
		else if (qxt_d().data->args.count() == 1)
		{
			qxt_d().data->ret = ReturnValue(qxt_d().data->args[0]);
		}
		else
		{
			qxt_d().data->ret = ReturnValue(100, "Error: Failed to generate the ReturnValue");
		}
	}
}

/**
 * Get the ReturnValue of the message. This function is only valid when Type is Return.
 * @return The ReturnValue of the message
 */
ReturnValue Message::returnValue() const
{
	QReadLocker lock(qxt_d().constMutex());
	return qxt_d().data->ret;
}

/**
 * Set the ReturnValue of the message. This function is only valid when Type is Return.
 * @param ret The new ReturnValue
 */
void Message::setReturnValue(const ReturnValue& ret)
{
	QWriteLocker lock(qxt_d().constMutex());
	qxt_d().data->ret = ret;
	/* This part is only needed for the original protocol */
	qxt_d().data->args.clear();
	if (ret.isError())
	{
		qxt_d().data->args.append(ret.errNumber());
		qxt_d().data->args.append(ret.errString());
	}
	else
	{
		qxt_d().data->args.append(ret);
	}
}

quint32 Message::service() const
{
	QReadLocker lock(qxt_d().constMutex());
	return qxt_d().data->service;
}

void Message::setService(quint32 id)
{
	QWriteLocker lock(qxt_d().constMutex());
	qxt_d().data->service = id;
}

qint64 Message::size() const
{
	QReadLocker lock(qxt_d().constMutex());
	QByteArray ba;
	QDataStream in(&ba, QIODevice::ReadWrite);
	in << *this;
	return ba.size();
}

/**
 * Deconstructor
 */
Message::~Message()
{
}

QDebug operator<<(QDebug dbg, const QtRpc::Message& msg)
{
	dbg.nospace() << "Message(Version: " << QString::number(msg.version(), 16) << ", ID: " << msg.id() << ", Type: " << msg.type();
	switch (msg.type())
	{
		case QtRpc::Message::Function:
		case QtRpc::Message::QtRpc:
		case QtRpc::Message::Event:
			if (msg.version() > 0)
				dbg.nospace() << ", ServiceID: " << msg.service();
			dbg.nospace() << ", Signature: " << msg.signature() << ", Arguments: " << msg.arguments();
			break;
		case QtRpc::Message::Return:
			dbg.nospace() << ", ReturnValue: " << msg.returnValue();
			break;
		case QtRpc::Message::Invalid:
		default:
			dbg.nospace() << ", Invalid";
			break;
	}
	dbg.nospace() << ")";
	return dbg.space();
}

QDebug operator<<(QDebug dbg, QtRpc::Message::Type msg)
{
	switch (msg)
	{
		case QtRpc::Message::Function:
			dbg << "Function call";
			break;
		case QtRpc::Message::QtRpc:
			dbg << "Protocol message";
			break;
		case QtRpc::Message::Event:
			dbg << "Event";
			break;
		case QtRpc::Message::Return:
			dbg << "Return value";
			break;
		case QtRpc::Message::Invalid:
		default:
			dbg << "Unknown or invalid message";
			break;
	}
	return dbg;
}

QDataStream& operator>> (QDataStream& s, QtRpc::Message& p)
{
	switch (p.version())
	{
		case 0x00000002: //Added magic number
		{
			quint32 i;
			s >> i;
			if(i != 0x1234abcd)
			{
				p.setType(Message::Invalid);
				return s;
			}
		}
		case 0x00000001: //updated packing functions
		{
			QtRpc::Message::Type type;
			s >> type;
			p.setType(type);
			switch (type)
			{
				case QtRpc::Message::Function:
				case QtRpc::Message::Event:
				{
					quint32 service;
					s >> service;
					p.setService(service);
				}
				case QtRpc::Message::QtRpc:
				{
					QtRpc::Signature func;
					QtRpc::Arguments args;
					uint id;
					s >> id >> func >> args;
					p.setId(id);
					p.setSignature(func);
					p.setArguments(args);
					break;
				}
				case QtRpc::Message::Return:
				{
					QtRpc::ReturnValue ret;
					uint id;
					s >> id >> ret;
					p.setId(id);
					p.setReturnValue(ret);
					break;
				}
				case QtRpc::Message::Invalid:
				default:
					qWarning() << "Reading invalid message from the network" << type;
					break;
			}
			break;
		}
		case 0x00000000: //original qtrpc2
		default:
		{
			uint id;
			uint type;
			QString func;
			QtRpc::Arguments args;
			s >> id >> type >> func >> args;
			quint32 service = 1;
			p = QtRpc::Message(id, static_cast<QtRpc::Message::Type>(type), QtRpc::Signature(func), args);
			p.setService(service);
			break;
		}
	}
	return s;
}

QDataStream& operator<< (QDataStream& s, const QtRpc::Message& p)
{
	const MessageData* priv = p.qxt_d().data.constData();
	switch (p.version())
	{
		case 0x00000002: //Added magic number
			s << static_cast<quint32>(0x1234abcd);
		case 0x00000001: //updated packing functions
		{
			s << priv->type;
			switch (p.type())
			{
				case QtRpc::Message::Function:
				case QtRpc::Message::Event:
					s << priv->service;
				case QtRpc::Message::QtRpc:
					s << priv->id << priv->func << priv->args;
					break;
				case QtRpc::Message::Return:
				{
					QtRpc::ReturnValue ret;
					s << priv->id << priv->ret;
					break;
				}
				case QtRpc::Message::Invalid:
				default:
					qWarning() << "Writing an invalid message to the network";
					break;
			}
			break;
		}
		case 0x00000000: //original qtrpc2
		default:
		{
			s << p.id() << ((uint)p.type()) << p.signature().toString() << p.arguments();
			break;
		}
	}
	return s;
}

QDataStream& operator>> (QDataStream& s, QtRpc::Message::Type &p)
{
	quint32 type;
	s >> type;
	switch (type)
	{
		case QtRpc::Message::Function:
		case QtRpc::Message::QtRpc:
		case QtRpc::Message::Event:
		case QtRpc::Message::Return:
			p = static_cast<QtRpc::Message::Type>(type);
			break;
		case QtRpc::Message::Invalid:
		default:
			p = QtRpc::Message::Invalid;
			break;
	}
	return s;
}

QDataStream& operator<< (QDataStream& s, const QtRpc::Message::Type p)
{
	s << static_cast<quint32>(p);
	return s;
}
