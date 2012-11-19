PROJECT_BEGIN(data_client EXECUTABLE)

USE_QT_LIB(NETWORK)

SET(SOURCES ${SOURCES}
        main.cpp
        basicservice.cpp
        dataservice.cpp
)

SET(HEADERS ${HEADERS}
        basicservice.h
        dataservice.h
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
