PROJECT_BEGIN(event_client EXECUTABLE)

USE_QT_LIB(Network)
USE_QT_LIB(Core)

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
