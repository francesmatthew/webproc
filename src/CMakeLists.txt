set(EXE_NAME webproc)

set(WEBPROC_SOURCES
    main.cpp
)

set(INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src
)

set(WEBPROC_LIBS
    process
    daemon
    argumentParser
    service
    client
)

add_subdirectory(argumentParser)
add_subdirectory(daemon)
add_subdirectory(process)
add_subdirectory(service)
add_subdirectory(client)

# Do not edit below this

add_executable(${EXE_NAME} ${WEBPROC_SOURCES})

target_include_directories(${EXE_NAME} PUBLIC ${INCLUDE_DIRS})

target_link_libraries(${EXE_NAME} PUBLIC
    ${WEBPROC_LIBS}
)

install(TARGETS ${EXE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
