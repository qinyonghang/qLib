cmake_minimum_required(VERSION 3.22)

project(qlib)
set (LIB_NAME qlib)

include(FetchContent)

if (NOT DEFINED CONFIG_PATH)
message(STATUS "Using default config.")
include(cmake/config.cmake)
else ()
message(STATUS "Using custom config: ${CONFIG_PATH}")
include(${CONFIG_PATH})
endif()

if (WITH_ONNXRUNTIME STREQUAL true)
    set(DFLAGS "${DFLAGS}" "WITH_ONNXRUNTIME")
endif()

if (WITH_NEON STREQUAL true)
    set(DFLAGS "${DFLAGS}" "WITH_NEON")
endif()

if (WITH_OPENCV STREQUAL true)
    set(DFLAGS "${DFLAGS}" "WITH_OPENCV")
endif()

if (WITH_BENCHMARK STREQUAL true)
    set(DFLAGS "${DFLAGS}" "WITH_BENCHMARK")
endif()

if (DUMP_TENSORS STREQUAL true)
    set(DFLAGS "${DFLAGS}" "DUMP_TENSORS")
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}.")
message(STATUS "DFLAGS: ${DFLAGS}")

file(GLOB source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
add_library(${LIB_NAME} STATIC ${source_files})

set_target_properties(${LIB_NAME} PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(${LIB_NAME} PUBLIC ${DFLAGS})

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
target_compile_options(${LIB_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-g -O0>
    -Wall
    -Wno-deprecated-declarations -Wno-int-in-bool-context -Wno-unused-function
    $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wl,-unresolved-symbols=ignore-in-shared-libs>
    -fPIC
)
endif ()

target_include_directories(${LIB_NAME} PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

if (WITH_BENCHMARK STREQUAL true)
    file(GLOB dhrystone2_files ${CMAKE_CURRENT_SOURCE_DIR}/src/dhrystone2/*.c)
    add_library(dhrystone2 STATIC ${dhrystone2_files})

    target_compile_options(dhrystone2 PRIVATE
        $<$<OR:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<COMPILE_LANG_AND_ID:CXX,Clang>>:-Wno-unused-variable -O3>
    )

    target_link_libraries(dhrystone2 PRIVATE -lc -lm)
    target_link_libraries(${LIB_NAME} PRIVATE dhrystone2)
endif ()


FetchContent_Declare(spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.15.0.tar.gz
    URL_HASH SHA256=9962648c9b4f1a7bbc76fd8d9172555bad1871fdb14ff4f842ef87949682caa5
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "Build shared libraries" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(SPDLOG_INSTALL ON CACHE BOOL "Install spdlog" FORCE)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(classic_benchmarks
    URL http://www.roylongbottom.org.uk/classic_benchmarks.tar.gz
    URL_HASH 1c69d7d5450bb1189e16638ed4e426f2329bc1d8203fac1e5ed14a2c3d916083
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/classic_benchmarks
    DOWNLOAD_EXTRACT_TIMESTAMP true
)

target_link_libraries(${LIB_NAME} PUBLIC spdlog::spdlog)

option(BUILD_TEST "Build tests" ON)
if (BUILD_TEST)
    enable_testing()

    add_executable(test_log src/test/QLogTest.cpp)
    target_link_libraries(test_log PRIVATE ${LIB_NAME})
endif ()

set(export_dest_dir ${CMAKE_INSTALL_LIBDIR}/cmake/${LIB_NAME})
set(config_targets_file "${LIB_NAME}ConfigTargets.cmake")
set(project_config_in "${PROJECT_SOURCE_DIR}/cmake/${LIB_NAME}Config.cmake.in")
set(project_config_out "${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}Config.cmake")

add_library(${LIB_NAME}_header_only INTERFACE)
target_include_directories(${LIB_NAME}_header_only INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
                                                          "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(${LIB_NAME}_header_only INTERFACE spdlog::spdlog_header_only)
target_compile_definitions(${LIB_NAME}_header_only INTERFACE ${DFLAGS})

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS ${LIB_NAME} ${LIB_NAME}_header_only
    EXPORT ${LIB_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

export(TARGETS ${LIB_NAME} ${LIB_NAME}_header_only NAMESPACE ${LIB_NAME}::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${config_targets_file}")
install(EXPORT ${LIB_NAME} DESTINATION ${export_dest_dir} NAMESPACE ${LIB_NAME}:: FILE ${config_targets_file})

include(CMakePackageConfigHelpers)
configure_package_config_file("${project_config_in}" "${project_config_out}" INSTALL_DESTINATION ${export_dest_dir})
install(FILES "${project_config_out}" DESTINATION "${export_dest_dir}")
