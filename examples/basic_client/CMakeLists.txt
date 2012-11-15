PROJECT_BEGIN(basic_client EXECUTABLE)

USE_QT_LIB(NETWORK)

SET(SOURCES ${SOURCES}
        main.cpp
)

SET(HEADERS ${HEADERS}
        
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
