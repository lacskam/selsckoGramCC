cmake_minimum_required(VERSION 3.16)

project(selsckoGram LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)
find_package(OpenSSL REQUIRED)






add_executable(selsckoGram main.cc
    session/session.h session/session.cc
    server/server.h server/server.cc
    net/proto/proto.h net/proto/proto.cc
    logs.h server/data/db/db_proccessing.h
    server/data/db/db_proccessing.cc
    server/data/db/db_config.h
    net/encry/encry.h net/encry/encry.cpp
    net/auth/auth_proccessing.h net/auth/auth_proccessing.cpp


)

target_include_directories(selsckoGram PRIVATE ${PQXX_INCLUDE_DIRS})

target_link_libraries(selsckoGram
    PRIVATE
    ${PQXX_LIBRARIES}
)
target_link_libraries(selsckoGram PRIVATE OpenSSL::SSL OpenSSL::Crypto)
include(GNUInstallDirs)
install(TARGETS selsckoGram
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(selsckoGram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/session
                                                ${CMAKE_CURRENT_SOURCE_DIR}/server
                                            ${CMAKE_CURRENT_SOURCE_DIR}/net/proto
                                        ${CMAKE_CURRENT_SOURCE_DIR}/server/data/db
                                    ${CMAKE_CURRENT_SOURCE_DIR}/net/auth
                                ${CMAKE_CURRENT_SOURCE_DIR}/net/encry
                            ${CMAKE_CURRENT_SOURCE_DIR}/)
add_subdirectory(selsck_test_client)
