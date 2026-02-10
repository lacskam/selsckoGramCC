cmake_minimum_required(VERSION 3.16)

project(selsckoGram LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(selsckoGram main.cpp
    session/session.h session/session.cpp
    server/server.h server/server.cpp
    net/proto/proto.h net/proto/proto.cpp
    logs.h

)

include(GNUInstallDirs)
install(TARGETS selsckoGram
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(selsckoGram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/session
                                                ${CMAKE_CURRENT_SOURCE_DIR}/server
                                            ${CMAKE_CURRENT_SOURCE_DIR}/net/proto)
