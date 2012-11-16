PROJECT_BEGIN(event_client EXECUTABLE)

USE_QT_LIB(NETWORK)

SET(SOURCES ${SOURCES}
        main.cpp
        timeservice.cpp
        timeobject.cpp
)

SET(HEADERS ${HEADERS}
        timeservice.h
        timeobject.h
)

# Include and link against qtrpc2
SET(INCLUDES ${INCLUDES}
        ../../include/
        ../../lib/
)
SET(LIBRARIES ${LIBRARIES}
        qtrpc2
)

PROJECT_END()
