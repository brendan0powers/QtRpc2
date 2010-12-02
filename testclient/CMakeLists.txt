PROJECT_BEGIN(qtrpc2.testclient EXECUTABLE)


SET(SOURCES ${SOURCES}
	main.cpp
	testclient.cpp
	testthread.cpp
	testcaller.cpp
	testsyncro.cpp
)

USE_QT_LIB(NETWORK)

SET(HEADERS ${HEADERS}
	testclient.h
	testthread.h
	testcaller.h
	testsyncro.h
)
SET(INCLUDES ${INCLUDES}
	../
	../include/
)

SET(LIBRARIES ${LIBRARIES}
	qtrpc2
)

PROJECT_END()

