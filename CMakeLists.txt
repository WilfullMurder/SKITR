cmake_minimum_required(VERSION 3.28)
project(skitr LANGUAGES CXX)

# Force policy minimum to 3.5 to satisfy older CMakeLists of deps
cmake_policy(VERSION 3.28)               # Already sets policies to 3.28 defaults
cmake_policy(SET CMP0077 NEW)            # optional: make FetchContent work nicely

# This ensures FetchContent deps with old cmake_minimum_required don't complain
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
        GIT_TAG asio-1-28-0
)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(asio nlohmann_json)

file(GLOB HEADERS_PUB_ROOT "include/skitr/*.h")
file(GLOB HEADERS_PUB_SUB "include/skitr/**/*.h")
set(HEADERS_PUB ${HEADERS_PUB_ROOT} ${HEADERS_PUB_SUB})

set(SOURCES
        ${HEADERS_PUB}
        src/runtime/dispatcher.cpp
        src/core/value.cpp
        src/registry/method_registry.cpp
        src/protocol/json_protocol.cpp
        src/transport/server.cpp
        src/main.cpp
        src/client.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX source FILES ${SOURCES})


# -----------------------------
# Library target
# -----------------------------
add_library(skitr ${SOURCES})

include(GenerateExportHeader)
generate_export_header(skitr
        EXPORT_FILE_NAME skitr/skitr_export.h
)

target_include_directories(skitr
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<BUILD_INTERFACE:${asio_SOURCE_DIR}/asio/include>
        $<BUILD_INTERFACE:${nlohmann_json_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
        src
)


target_compile_definitions(skitr PUBLIC ASIO_STANDALONE)

if (MSVC)
    target_compile_options(skitr PRIVATE /W4)
else()
    target_compile_options(skitr PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (WIN32)
    target_link_libraries(skitr PUBLIC ws2_32 mswsock)
endif()

add_library(skitr::skitr ALIAS skitr)

# -----------------------------
# Export CMake package for find_package()
# -----------------------------
include(CMakePackageConfigHelpers)

set(SKITR_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_DATADIR}/skitr/cmake")

install(TARGETS skitr EXPORT skitrTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT skitrTargets
        FILE skitrTargets.cmake
        NAMESPACE skitr::
        DESTINATION "${SKITR_INSTALL_CMAKE_DIR}"
)

configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/skitrConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/skitrConfig.cmake"
        INSTALL_DESTINATION "${SKITR_INSTALL_CMAKE_DIR}"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/skitrConfig.cmake"
        DESTINATION "${SKITR_INSTALL_CMAKE_DIR}"
)

# -----------------------------
# Optional executable for testing
# -----------------------------
add_executable(skitr_server src/main.cpp)
target_link_libraries(skitr_server PRIVATE skitr)
target_include_directories(skitr_server PRIVATE
        ${asio_SOURCE_DIR}/asio/include
        ${nlohmann_json_SOURCE_DIR}/include
)

add_executable(skitr_client src/client.cpp)
target_link_libraries(skitr_client PRIVATE skitr)
target_include_directories(skitr_server PRIVATE
        ${asio_SOURCE_DIR}/asio/include
)

target_compile_definitions(skitr_server PRIVATE ASIO_STANDALONE)
if(WIN32)
    target_link_libraries(skitr_server PRIVATE ws2_32 mswsock)
endif()

enable_testing()
add_subdirectory(tests)