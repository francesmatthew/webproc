set(EXE_NAME webproc)

set(WEBPROC_SOURCES
    main.cpp
)

# Do not edit below this

add_executable(${EXE_NAME} ${WEBPROC_SOURCES})

target_include_directories(${EXE_NAME} PUBLIC
                           "${PROJECT_SOURCE_DIR}"
                           )

install(TARGETS ${EXE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
