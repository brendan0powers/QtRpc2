PROJECT_BEGIN(data_server EXECUTABLE)

USE_QT_LIB(Network)
USE_QT_LIB(Core)

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
