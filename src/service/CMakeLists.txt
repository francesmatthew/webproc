set(LIB_NAME service)

set(SOURCES
    Service.cpp
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src
)

set(LIBS
)

# Do not edit below this

add_library(${LIB_NAME} STATIC ${SOURCES})

target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${LIB_NAME} PUBLIC ${LIBS})
