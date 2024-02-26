set(LIB_NAME daemon)

set(SOURCES
    Daemon.cpp
    Logger.cpp
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src
)

set(LIBS
    process
)

# Do not edit below this

add_library(${LIB_NAME} STATIC ${SOURCES})

target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${LIB_NAME} PUBLIC ${LIBS})
