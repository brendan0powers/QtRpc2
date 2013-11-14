PROJECT_BEGIN(qtrpc2 LIBRARY)

ADD_DEFINITIONS(-DQTRPC2_BUILD_LIB)

IF(WIN32)
			cmake_policy(SET CMP0020 OLD)
ENDIF(WIN32)

USE_QT_LIB(Network)
SET(QT_DONT_USE_QTGUI true)

SET(HEADERS ${HEADERS}
	authtoken.h
	returnvalue.h
	returnvalue_p.h
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
	server.h
	serverprotocolinstancebase.h
	serverprotocolinstanceiodevice.h
	serverprotocolinstanceiodevice_p.h
	serverprotocolinstancetcp.h
	serverprotocolinstancetcp_p.h
	serverprotocollistenertcp.h
	serverthread.h
	serviceproxy.h
	signature.h
	signature_p.h
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
	authtoken.cpp
	servicefactoryparent.cpp
)

INCLUDE_DIRECTORIES(../include/)

IF(APPLE)
	SET(BONJOUR_FOUND TRUE) #Apple always has bonjour
	SET(SOURCES ${SOURCES}
			qxtmdns_bonjour.cpp
	)
	SET(HEADERS ${HEADERS}
			qxtmdns_bonjour.h
	)
ENDIF(APPLE)

IF(WIN32)
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
		
		SET(SOURCES ${SOURCES}
			qxtmdns_bonjour.cpp
		)
		SET(HEADERS ${HEADERS}
			qxtmdns_bonjour.h
		)
		
		SET(LIBRARIES ${LIBRARIES}
			${BONJOUR_LIBRARY}
		)
		INCLUDE_DIRECTORIES(
			${BONJOUR_INCLUDE_DIR}
		)
	ELSE (BONJOUR_FOUND)
		MESSAGE(STATUS "Could not find Bonjour, leaving out service discovery support: (${BONJOUR_LIBRARY}, ${BONJOUR_INCLUDE_DIR}")
	ENDIF (BONJOUR_FOUND)
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
	SET(BONJOUR_FOUND TRUE) #TODO: make avahi for linux an option
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

IF(BONJOUR_FOUND)
	add_definitions(-DBONJOUR_FOUND)

	SET(HEADERS ${HEADERS}
		qxtdiscoverableservice.h
		qxtdiscoverableservice_p.h
		qxtservicebrowser.h
		qxtservicebrowser_p.h
		servicefinder.h
		servicefinder_p.h
		servicepublisher.h
	)
	
	SET(SOURCES ${SOURCES}
		qxtdiscoverableservice.cpp
		qxtdiscoverableservicename.cpp
		qxtservicebrowser.cpp
		servicefinder.cpp
		servicepublisher.cpp
	)
ENDIF()

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
