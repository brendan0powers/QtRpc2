PROJECT_BEGIN(discovery_client EXECUTABLE)

USE_QT_LIB(Network)
USE_QT_LIB(Core)

SET(SOURCES ${SOURCES}
        main.cpp
        basicservice.cpp
        testobject.cpp
)

SET(HEADERS ${HEADERS}
        basicservice.h
        testobject.h
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
