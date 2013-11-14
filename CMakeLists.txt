cmake_minimum_required( VERSION 2.8.9 )

IF(WIN32)
	cmake_policy(SET CMP0020 OLD)
ENDIF()

Project(QtRPC2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_AUTOMOC ON)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
SET_PROPERTY(GLOBAL PROPERTY AUTOMOC_FOLDER automoc)

IF(NOT ${QT_ROOT} STREQUAL "")
	SET(CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake)
ENDIF()

find_package (Qt5Core)

if(NOT Qt5Core_FOUND)
	MESSAGE(FATAL_ERROR "Qt5 not found - Please specify -DQT_ROOT=path/to/qt_root")
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)

IF(UNIX)
	set(CMAKE_CXX_FLAGS "-Wall")
	set(CMAKE_C_FLAGS "-Wall")
ENDIF(UNIX)

#############################################
################### MACROS ##################
#############################################

MACRO(USE_QT_LIB lib)
	LIST(APPEND QT_LIBRARIES ${lib})
ENDMACRO(USE_QT_LIB)

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
	SET(QT_LIBRARIES "")
ENDMACRO(PROJECT_BEGIN)

MACRO(PROJECT_END)
	SET(INCLUDES ${INCLUDES}
		./
	)
	IF(NOT ${QT_USE_FILE} STREQUAL "")
		include ( ${QT_USE_FILE} )
	ENDIF()

	QT5_ADD_RESOURCES(RESOURCES_SRCS ${RESOURCES})

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
		ADD_LIBRARY(${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARY_TYPE} ${SOURCES} ${HEADERS} ${MOC_SOURCES} ${FORMS_UI})
		SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
					VERSION 1.0.0
					SOVERSION 1)
	ELSE("${PROJECT_APP_TYPE}" MATCHES "LIBRARY")
		IF("${PROJECT_APP_TYPE}" MATCHES "EXECUTABLE")
			MESSAGE(STATUS "Building ${PROJECT_NAME} as an executable.")
		ELSE("${PROJECT_APP_TYPE}" MATCHES "EXECUTABLE")
			MESSAGE(FATAL_ERROR "Project type must be either LIBRARY or EXECUTABLE, found ${PROJECT_APP_TYPE}. Please fix the PROJECT_BEGIN macro in CMakeLists.txt")
		ENDIF("${PROJECT_APP_TYPE}" MATCHES "EXECUTABLE")
		ADD_EXECUTABLE(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${MOC_SOURCES} ${FORMS_UI})
		IF(WIN32)
			cmake_policy(SET CMP0020 OLD)
			target_link_libraries(${PROJECT_NAME} Qt5::WinMain)
		ENDIF(WIN32)
	ENDIF("${PROJECT_APP_TYPE}" MATCHES "LIBRARY")

	TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARIES})
	INCLUDE_DIRECTORIES(
		${INCLUDES}
		${QT_INCLUDE_DIR}
	)

	QT5_USE_MODULES(${PROJECT_NAME} ${QT_LIBRARIES})

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


IF(WIN32)
	cmake_policy(SET CMP0020 OLD)
	
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
ELSE()
	SET(BONJOUR_FOUND TRUE) #For other platforms, we currently assume bonjour is found
ENDIF(WIN32)

#############################################
################### PROJECT##################
#############################################


add_subdirectory("lib")
add_subdirectory("examples/basic_client")
add_subdirectory("examples/basic_server")
add_subdirectory("examples/event_client")
add_subdirectory("examples/event_server")
add_subdirectory("examples/async_client")
add_subdirectory("examples/async_server")
add_subdirectory("examples/auth_client")
add_subdirectory("examples/auth_server")
add_subdirectory("examples/md5auth_client")
add_subdirectory("examples/md5auth_server")
add_subdirectory("examples/ssl_client")
add_subdirectory("examples/ssl_server")
add_subdirectory("examples/thread_client")
add_subdirectory("examples/thread_server")
add_subdirectory("examples/data_client")
add_subdirectory("examples/data_server")

IF(BONJOUR_FOUND)
	add_subdirectory("examples/discovery_client")
	add_subdirectory("examples/discovery_server")
ENDIF()
