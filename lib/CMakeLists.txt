PROJECT_BEGIN(qtrpc2 LIBRARY)

SET(CMAKE_CXX_FLAGS "-D QTRPC2_BUILD_LIB")

USE_QT_LIB(NETWORK)
SET(QT_DONT_USE_QTGUI true)

SET(HEADERS ${HEADERS}
	clientmessagebus.h
	clientmessagebus_p.h
	clientprotocolbase.h
	clientprotocoliodevice.h
	clientprotocoliodevice_p.h
	clientprotocoltcp.h
	clientprotocoltcp_p.h
	clientprotocoltest.h
	clientprotocolthread.h
	clientproxy.h
	clientproxy_p.h
	proxybase.h
	qxtdiscoverableservice.h
	qxtdiscoverableservice_p.h
	qxtservicebrowser.h
	qxtservicebrowser_p.h
	server.h
	serverprotocolinstancebase.h
	serverprotocolinstanceiodevice.h
	serverprotocolinstanceiodevice_p.h
	serverprotocolinstancetcp.h
	serverprotocolinstancetcp_p.h
	serverprotocollistenertcp.h
	serverthread.h
	servicefinder.h
	servicefinder_p.h
	serviceproxy.h
	servicepublisher.h
)

SET(SOURCES ${SOURCES}
	returnvalue.cpp
	signature.cpp
	proxybase.cpp
	clientmessagebus.cpp
	clientprotocolthread.cpp
	clientprotocolbase.cpp
	clientproxy.cpp
	server.cpp
	serviceproxy.cpp
	clientprotocoltest.cpp
	serverprotocolinstancebase.cpp
	serverprotocollistenerbase.cpp
	serverthread.cpp
	serverprotocollistenertcp.cpp
	serverprotocolinstanceiodevice.cpp
	message.cpp
	serverprotocolinstancetcp.cpp
	clientprotocoltcp.cpp
	clientprotocoliodevice.cpp
	serverprotocollistenerprocess.cpp
	servicepublisher.cpp
	servicefinder.cpp
	authtoken.cpp
	servicefactoryparent.cpp
	qxtdiscoverableservice.cpp
	qxtdiscoverableservicename.cpp
	qxtservicebrowser.cpp
)

INCLUDE_DIRECTORIES(../include/)

IF(APPLE)
SET(SOURCES ${SOURCES}
        qxtmdns_bonjour.cpp
)
SET(HEADERS ${HEADERS}
        qxtmdns_bonjour.h
)

ENDIF(APPLE)

IF(WIN32)
SET(SOURCES ${SOURCES}
	qxtmdns_bonjour.cpp
)
SET(HEADERS ${HEADERS}
	qxtmdns_bonjour.h
)

FIND_LIBRARY(BONJOUR_LIBRARY NAMES dnssd.lib PATHS
	"C:/Program Files/Bonjour SDK/Lib/Win32/"
	"C:/Bonjour SDK/Lib/Win32/"
)

FIND_PATH(BONJOUR_INCLUDE_DIR dns_sd.h
	"C:/Program Files/Bonjour SDK/Include/"
	"C:/Bonjour SDK/Include/"
)

IF (BONJOUR_INCLUDE_DIR AND BONJOUR_LIBRARY)
   SET(BONJOUR_FOUND TRUE)
ENDIF (BONJOUR_INCLUDE_DIR AND BONJOUR_LIBRARY)


IF (BONJOUR_FOUND)
	MESSAGE(STATUS "Found Bonjour: ${BONJOUR_LIBRARY}")
ELSE (BONJOUR_FOUND)
	MESSAGE(FATAL_ERROR "Could not find Bonjour (${BONJOUR_LIBRARY}, ${BONJOUR_INCLUDE_DIR}")
ENDIF (BONJOUR_FOUND)

SET(LIBRARIES ${LIBRARIES}
	${BONJOUR_LIBRARY}
)
INCLUDE_DIRECTORIES(
	${BONJOUR_INCLUDE_DIR}
)


ENDIF(WIN32)

IF(UNIX)
SET(SOURCES ${SOURCES}
	clientprotocolsocket.cpp
	serverprotocollistenersocket.cpp
	serverprotocolinstancesocket.cpp
)
SET(HEADERS ${HEADERS}
	clientprotocolsocket.h
	serverprotocollistenersocket.h
	serverprotocolinstancesocket.h
)

ENDIF(UNIX)

IF(UNIX AND NOT APPLE)

SET(SOURCES ${SOURCES}
	qxtmdns_avahi.cpp
	qxtavahipoll.cpp
)
SET(HEADERS ${HEADERS}
	qxtmdns_avahi.h
	qxtavahipoll.h
	qxtavahipoll_p.h
)


SET(LIBRARIES ${LIBRARIES}
        avahi-client
        avahi-common
        dns_sd
)

ENDIF(UNIX AND NOT APPLE)

PROJECT_END()

IF(APPLE)

INSTALL(TARGETS qtrpc2 DESTINATION ${CMAKE_INSTALL_PREFIX}/rdsconsole.app/Contents/MacOS/)

ELSE(APPLE)

INSTALL(TARGETS qtrpc2
	BUNDLE DESTINATION .
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

ENDIF(APPLE)

INSTALL_HEADERS("qtrpc2/")
