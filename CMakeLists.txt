cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR)

include(CheckCXXCompilerFlag)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(WARNING "In source builds are not allowed\n\n")
  message(STATUS "Stopping CMake and cleaning up generated files")
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/CMakeFiles")
  # Crash CMake since it doesn't have a way to stop without leaving behind files
  math(EXPR Crash 0/0)
  message(FATAL_ERROR "CMake should have crashed - this is a failsafe in case the call used to trigger the crash gets fixed.")
endif()

project(VELOX)

find_package(Threads)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang|GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")

  add_definitions(-DCATCH_CONFIG_SFINAE)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-exit-time-destructors")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors -Wno-padded")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wold-style-cast -Wuseless-cast")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wctor-dtor-privacy -Wmissing-include-dirs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-enum -Wuninitialized -Winit-self")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal -Wundef -Wshadow")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunsafe-loop-optimizations -Wcast-qual")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align -Wconversion -Wsign-conversion")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wzero-as-null-pointer-constant")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsizeof-pointer-memaccess")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op -Wredundant-decls -Winvalid-pch")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvector-operation-performance")
endif()

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
endif()
  
add_custom_target(run
  COMMAND velox_tests 
  DEPENDS velox_tests
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

include_directories(${VELOX_SOURCE_DIR}/include)
include_directories(${VELOX_SOURCE_DIR}/tests)

set(HEADERS
  include/benchmark.h
  include/bootstrap.h
  include/format.h
  include/fp_range.h
  include/html_reporter.h
  include/iterator_base.h
  include/iters_for_duration.h
  include/kde.h
  include/measurement.h
  include/multi_reporter.h
  include/outliers.h
  include/point.h
  include/regression.h
  include/reporter.h
  include/stats.h
  include/stopwatch.h
  include/text_reporter.h
  include/util.h
  include/velox.h
  include/velox_config.h
)

set(SOURCE
  tests/main.cpp 
  tests/stats.cpp
  tests/stopwatch.cpp
  tests/util.cpp
  tests/fp_range.cpp
  tests/outliers.cpp
  tests/bootstrap.cpp
  tests/kde.cpp
  tests/regression.cpp
  tests/format.cpp
  tests/multiple_definitions_one.cpp
  tests/multiple_definitions_two.cpp
)

source_group(headers FILES ${HEADERS}) 

add_executable(velox_tests ${HEADERS} ${SOURCE})

target_link_libraries(velox_tests ${CMAKE_THREAD_LIBS_INIT})
