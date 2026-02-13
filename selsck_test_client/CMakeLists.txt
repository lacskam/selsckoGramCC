cmake_minimum_required(VERSION 3.16)

project(selsck_test_client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(GNUInstallDirs)




find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)
find_package(OpenSSL REQUIRED)






add_executable(selsck_test_client main.cpp
    ../net/proto/proto.h ../net/proto/proto.cc
    ../logs.h ../net/encry/encry.h ../net/encry/encry.cpp
)

target_include_directories(selsck_test_client PRIVATE ${PQXX_INCLUDE_DIRS})

target_link_libraries(selsck_test_client
    PRIVATE
    ${PQXX_LIBRARIES}
)
target_link_libraries(selsck_test_client PRIVATE OpenSSL::SSL OpenSSL::Crypto)
include(GNUInstallDirs)
install(TARGETS selsck_test_client
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_include_directories(selsckoGram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/net/proto ${CMAKE_CURRENT_SOURCE_DIR}../net/encry)

