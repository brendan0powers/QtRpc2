PROJECT_BEGIN(qtrpc2.testserver EXECUTABLE)

SET(SOURCES ${SOURCES}
	main.cpp
	testserver.cpp
	callerservice.cpp
	testsyncroserver.cpp
)

SET(HEADERS ${HEADERS}
	testserver.h
	callerservice.h
	testsyncroserver.h
)

USE_QT_LIB(Network)
USE_QT_LIB(Core)

SET(INCLUDES ${INCLUDES}
	../
	../include/
)

SET(LIBRARIES ${LIBRARIES}
	qtrpc2
)

PROJECT_END()

