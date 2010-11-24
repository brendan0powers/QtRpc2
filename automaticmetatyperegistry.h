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
#ifndef QTRPCAUTOMATICMETATYPEREGISTRY_H
#define QTRPCAUTOMATICMETATYPEREGISTRY_H

#include <QMetaType>

namespace QtRpc
{

/**
	@author Chris Vickery <chris@resara.com>
*/
template<typename T>
class AutomaticMetatypeRegistry
{
public:
	AutomaticMetatypeRegistry(const char* name)
	{
		qRegisterMetaType<T>(name);
		qRegisterMetaTypeStreamOperators<T>(name);
	}
	~AutomaticMetatypeRegistry()
	{
	}
};

}

#define __QTRPC_REGISTER_METATYPE(x,y) static QtRpc::AutomaticMetatypeRegistry<x> __qtrpc_typeRegistry##y (#x);
#define ___QTRPC_REGISTER_METATYPE(x, bleh) __QTRPC_REGISTER_METATYPE(x, bleh )
#define QTRPC_REGISTER_METATYPE(x)\
	___QTRPC_REGISTER_METATYPE(x, __LINE__ );

#endif
