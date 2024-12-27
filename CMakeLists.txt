cmake_minimum_required(VERSION 3.22)

project(common)
set (LIB_NAME common)

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

# find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
# message("Python_INCLUDE_DIRS=" ${Python3_INCLUDE_DIRS})
# message("Python_LIBRARIES=" ${Python3_LIBRARIES})

# find_package(Boost REQUIRED COMPONENTS python)
# message("Boost version: ${Boost_VERSION}")
# message("Boost_INCLUDE_DIR=" ${Boost_INCLUDE_DIR})
# message("Boost_LIBRARIES=" ${Boost_LIBRARIES})

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
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (WITH_BENCHMARK STREQUAL true)
    file(GLOB dhrystone2_files ${CMAKE_CURRENT_SOURCE_DIR}/src/dhrystone2/*.c)
    add_library(dhrystone2 STATIC ${dhrystone2_files})

    target_compile_options(dhrystone2 PRIVATE
        $<$<OR:$<COMPILE_LANG_AND_ID:CXX,GNU>,$<COMPILE_LANG_AND_ID:CXX,Clang>>:-Wno-unused-variable -O3>
    )

    target_link_libraries(dhrystone2 PRIVATE -lc -lm)
    target_link_libraries(${LIB_NAME} PUBLIC dhrystone2)
endif ()


FetchContent_Declare(spdlog
    URL https://github.com/gabime/spdlog/archive/refs/tags/v1.15.0.tar.gz
    URL_HASH SHA256=9962648c9b4f1a7bbc76fd8d9172555bad1871fdb14ff4f842ef87949682caa5
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/spdlog
)
set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "Build shared libraries" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(classic_benchmarks
    URL http://www.roylongbottom.org.uk/classic_benchmarks.tar.gz
    URL_HASH 1c69d7d5450bb1189e16638ed4e426f2329bc1d8203fac1e5ed14a2c3d916083
    DOWNLOAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/classic_benchmarks
)

target_link_libraries(${LIB_NAME} PUBLIC spdlog::spdlog)
