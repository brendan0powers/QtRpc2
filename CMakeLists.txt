cmake_minimum_required( VERSION 2.6 )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package ( Qt4 REQUIRED )
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)

IF(UNIX)
	set(CMAKE_CXX_FLAGS "-Wall")
	set(CMAKE_C_FLAGS "-Wall")
ENDIF(UNIX)

#############################################
################### MACROS ##################
#############################################

MACRO(USE_QT_LIB lib)
	SET(QT_USE_QT${lib} true)
	SET(LIBRARIES ${LIBRARIES} ${QT_QT${lib}_LIBRARIES})
	SET(INCLUDES ${INCLUDES} ${QT_QT${lib}_INCLUDE_DIR})
ENDMACRO(USE_QT_LIB)

MACRO(CALL_PROJECT_HACK name)
#	PROJECT(${name})
ENDMACRO(CALL_PROJECT_HACK)

MACRO(PROJECT_BEGIN name apptype)
	SET(PROJECT_NAME ${name})
	SET(PROJECT_APP_TYPE ${apptype})
	IF("${PROJECT_APP_TYPE}" MATCHES "LIBRARY")
		IF("${PROJECT_APP_TYPE}" MATCHES "STATIC_LIBRARY")
			SET(BUILD_${PROJECT_NAME}_STATIC ON)
			OPTION(BUILD_${PROJECT_NAME}_STATIC "Build ${PROJECT_NAME} statically." ON)
		ELSE("${PROJECT_APP_TYPE}" MATCHES "STATIC_LIBRARY")
			OPTION(BUILD_${PROJECT_NAME}_STATIC "Build ${PROJECT_NAME} statically." OFF)
		ENDIF("${PROJECT_APP_TYPE}" MATCHES "STATIC_LIBRARY")
	ENDIF("${PROJECT_APP_TYPE}" MATCHES "LIBRARY")
	CALL_PROJECT_HACK(${PROJECT_NAME})
ENDMACRO(PROJECT_BEGIN)

MACRO(PROJECT_END)
	SET(INCLUDES ${INCLUDES}
		./
	)
	include ( ${QT_USE_FILE} )

	QT4_WRAP_CPP(MOC_SOURCES ${HEADERS})
	QT4_WRAP_UI(FORMS_UI ${FORMS})
	QT4_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})
	SET(SOURCES ${SOURCES} ${RESOURCES_SRCS})

	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})

	IF("${PROJECT_APP_TYPE}" MATCHES "LIBRARY")
		IF(BUILD_${PROJECT_NAME}_STATIC)
			SET(${PROJECT_NAME}_LIBRARY_TYPE STATIC)
		ELSE(BUILD_${PROJECT_NAME}_STATIC)
			SET(${PROJECT_NAME}_LIBRARY_TYPE SHARED)
		ENDIF(BUILD_${PROJECT_NAME}_STATIC)

		IF("${${PROJECT_NAME}_LIBRARY_TYPE}" MATCHES "STATIC")
			MESSAGE(STATUS "Building ${PROJECT_NAME} as a static library.")
		ELSE("${${PROJECT_NAME}_LIBRARY_TYPE}" MATCHES "STATIC")
			IF("${${PROJECT_NAME}_LIBRARY_TYPE}" MATCHES "SHARED")
				MESSAGE(STATUS "Building ${PROJECT_NAME} as a shared library.")
				IF(UNIX)
					ADD_CUSTOM_TARGET(
							lib${PROJECT_NAME}.so.1 ALL
							ln -fs lib${PROJECT_NAME}.so.1.0.0 lib${PROJECT_NAME}.so.1
							DEPENDS ${PROJECT_NAME}
					)
					INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib${PROJECT_NAME}.so.1
						DESTINATION lib
					)
				ENDIF(UNIX)
			ELSE("${${PROJECT_NAME}_LIBRARY_TYPE}" MATCHES "SHARED")
				MESSAGE(FATAL_ERROR "Library type must be either SHARED or STATIC, found ${${PROJECT_NAME}_LIBRARY_TYPE}. Please fix the PROJECT_BEGIN macro in CMakeLists.txt")
			ENDIF("${${PROJECT_NAME}_LIBRARY_TYPE}" MATCHES "SHARED")
		ENDIF("${${PROJECT_NAME}_LIBRARY_TYPE}" MATCHES "STATIC")

		ADD_LIBRARY(${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARY_TYPE} ${SOURCES} ${MOC_SOURCES} ${FORMS_UI})
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
					VERSION 1.0.0
					SOVERSION 1)
	ELSE("${PROJECT_APP_TYPE}" MATCHES "LIBRARY")
		IF("${PROJECT_APP_TYPE}" MATCHES "EXECUTABLE")
			MESSAGE(STATUS "Building ${PROJECT_NAME} as an executable.")
		ELSE("${PROJECT_APP_TYPE}" MATCHES "EXECUTABLE")
			MESSAGE(FATAL_ERROR "Project type must be either LIBRARY or EXECUTABLE, found ${PROJECT_APP_TYPE}. Please fix the PROJECT_BEGIN macro in CMakeLists.txt")
		ENDIF("${PROJECT_APP_TYPE}" MATCHES "EXECUTABLE")
		IF(WIN32)
			USE_QT_LIB(MAIN)
		ENDIF(WIN32)
		ADD_EXECUTABLE(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCES} ${MOC_SOURCES} ${FORMS_UI})
	ENDIF("${PROJECT_APP_TYPE}" MATCHES "LIBRARY")

	SET(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})
	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES})
	INCLUDE_DIRECTORIES(
		${INCLUDES}
		${QT_INCLUDE_DIR}
	)

	TARGET_LINK_LIBRARIES(${PROJECT_NAME}
		${QT_LIBRARIES}
	)
	SET(SOURCES)
	SET(HEADERS)
	SET(LIBRARIES)
	SET(INCLUDES)
	SET(RESOURCES)
	SET(FORMS)
	SET(PROJECT_NAME)
	SET(PROJECT_APP_TYPE)
ENDMACRO(PROJECT_END)


MACRO(INSTALL_HEADERS path)
FILE(GLOB CONVHEADERS ./include/[A-Z]*)
FOREACH(CH ${CONVHEADERS})
	STRING(REGEX REPLACE ".*/" "" HEADERNAME ${CH})
	SET(HEADERNAME ${HEADERNAME}.h)
	STRING(TOLOWER ${HEADERNAME} HEADERNAME)
	INSTALL(FILES ${CH} ${HEADERNAME}
		DESTINATION include/${path})
ENDFOREACH()
ENDMACRO(INSTALL_HEADERS)

FOREACH(SUBDIR ${SUBDIRECTORIES})
	add_subdirectory(${SUBDIR})
ENDFOREACH()

IF(UNIX AND NOT APPLE)
FOREACH(SUBDIR ${SUBDIRECTORIES_UNIX})
	add_subdirectory(${SUBDIR})
ENDFOREACH()
ENDIF(UNIX AND NOT APPLE)


#############################################
################### PROJECT##################
#############################################


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

INCLUDE_DIRECTORIES(./include/)

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

