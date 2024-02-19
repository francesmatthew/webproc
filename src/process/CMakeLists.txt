set(LIB_NAME process)

set(SOURCES
    CPipe.cpp
    Process.cpp
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}
)

# Do not edit below this

add_library(${LIB_NAME} STATIC ${SOURCES})

target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIRS})
