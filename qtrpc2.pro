TEMPLATE = lib

CONFIG += dll \
 debug

SOURCES += returnvalue.cpp \
signature.cpp \
proxybase.cpp \
clientmessagebus.cpp \
clientprotocolthread.cpp \
clientprotocolbase.cpp \
 clientproxy.cpp \
 server.cpp \
 serviceproxy.cpp \
 clientprotocoltest.cpp \
 serverprotocolinstancebase.cpp \
 serverprotocollistenerbase.cpp \
 serverthread.cpp \
 serverprotocollistenertcp.cpp \
 serverprotocolinstanceiodevice.cpp \
 message.cpp \
 serverprotocolinstancetcp.cpp \
 clientprotocoltcp.cpp \
 clientprotocoliodevice.cpp \
 serverprotocollistenerprocess.cpp \
 servicepublisher.cpp \
 servicefinder.cpp \
 authtoken.cpp \
 servicefactoryparent.cpp \
 qxtdiscoverableservice.cpp \
 qxtdiscoverableservicename.cpp \
 qxtservicebrowser.cpp


unix {
LIBS += -ldns_sd
    SOURCES += qxtmdns_avahi.cpp \
                qxtavahipoll.cpp
    HEADERS += qxtmdns_avahi.h qxtmdns_avahi_p.h \
 qxtavahipoll.h \
 qxtavahipoll_p.h
    LIBS += -lavahi-client -lavahi-common

SOURCES += clientprotocolsocket.cpp \
 serverprotocollistenersocket.cpp \
 serverprotocolinstancesocket.cpp
HEADERS += serverprotocollistenersocket_p.h \
 serverprotocolinstancesocket_p.h \
 clientprotocolsocket_p.h \
 serverprotocollistenersocket.h \
 serverprotocolinstancesocket.h \
 clientprotocolsocket.h
}
win32 {
    SOURCES +=  qxtmdns_bonjour.cpp
    HEADERS += qxtmdns_bonjour.h
    INCLUDEPATH += $$quote(C:/Program Files/Bonjour SDK/Include/)
    LIBS += $$quote(C:/Program Files/Bonjour SDK/Lib/Win32/dnssd.lib)
    #LIBS += $$quote(C:/rds/qtrpc2/dnssd.dll)
}

HEADERS += returnvalue.h \
signature.h \
proxybase.h \
clientprotocolthread.h \
clientmessagebus.h \
clientprotocolbase.h \
 clientproxy.h \
 server.h \
 serviceproxy.h \
 clientprotocoltest.h \
 serverprotocolinstancebase.h \
 serverprotocollistenerbase.h \
 serverthread.h \
 serverprotocollistenertcp.h \
 serverprotocolinstanceiodevice.h \
 message.h \
 serverprotocolinstancetcp.h \
 clientprotocoltcp.h \
 clientprotocoliodevice.h \
 serverprotocollistenerprocess.h \
 servicefactory.h \
 servicepublisher.h \
 servicepublisher_p.h \
 servicefinder.h \
 servicefinder_p.h \
 server_p.h \
 clientproxy_p.h \
 clientmessagebus_p.h \
 clientprotocolbase_p.h \
 clientprotocoliodevice_p.h \
 clientprotocoltcp_p.h \
 clientprotocolthread_p.h \
 message_p.h \
 proxybase_p.h \
 returnvalue_p.h \
 serverprotocolinstancebase_p.h \
 serverprotocolinstanceiodevice_p.h \
 serverprotocolinstancetcp_p.h \
 serverprotocollistenerbase_p.h \
 serverprotocollistenerprocess_p.h \
 serverprotocollistenertcp_p.h \
 serverthread_p.h \
 signature_p.h \
 serviceproxy_p.h \
 clientprotocoltest_p.h \
 qtrpcprivate.h \
 authtoken.h \
 authtoken_p.h \
 servicefactoryparent_p.h \
 automaticmetatyperegistry.h \
 qxtdiscoverableservice.h \
 qxtdiscoverableservicename.h \
 qxtdiscoverableservice_p.h \
 qxtservicebrowser.h \
 qxtservicebrowser_p.h \
 qxtzeroconf.h \
 qxtmdns.h \
 sleeper.h \
    qtrpcglobal.h

DISTFILES += ReturnValue \
Signature \
TmpArgument \
ClientProtocolThread \
ClientMessageBus \
ClientProtocolBase \
ProxyBase \
 ClientProtocolTest \
 Server \
 ServiceProxy \
 ServerProtocolListenerBase \
 ServerProtocolInstanceBase \
 ServerThread \
 ServerProtocolListenerTcp \
 Message \
 ServerProtocolInstanceIODevice \
 ServerProtocolInstanceTcp \
 ClientProtocolTcp \
 ClientProtocolIODevice \
 ClientProtocolSocket \
 ServerProtocolListenerProcess \
 ServiceFactory \
 ServicePublisher \
 ServiceFinder \
 QSharedPointer \
 AuthToken \
 QtRpcSharedPointer \
 AutomaticMetatypeRegistry
CONFIG -= release \
exceptions \
stl

QT += network

DEFINES += QTRPC2_BUILD_LIB
INCLUDEPATH += ../includes/ ./
