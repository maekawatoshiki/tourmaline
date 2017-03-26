project(tourmaline)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE 1)
enable_testing()

set(LLVM_CONFIG "llvm-config-3.8")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
add_definitions("-O3 -std=c++11")
add_definitions("-D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS") # for LLVM

execute_process(
  COMMAND
  ${LLVM_CONFIG} --includedir
  OUTPUT_VARIABLE
  LLVM_INCLUDES
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND
  ${LLVM_CONFIG} --libdir
  OUTPUT_VARIABLE
  LLVM_LIBRARIES
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND
  ${LLVM_CONFIG} --libs all
  OUTPUT_VARIABLE
  LLVM_LIBS_ALL_STRING
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND
  ${LLVM_CONFIG} --system-libs
  OUTPUT_VARIABLE
  LLVM_SYSTEM_LIBS_STRING
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(LLVM_LIBS_ALL)
foreach (l ${LLVM_SYSTEM_LIBS_STRING} ${LLVM_LIBS_ALL_STRING})
    string(REGEX REPLACE "-l([^ ]+)" "-l\\1" lib_name ${l})
    list(APPEND LLVM_LIBS_ALL ${lib_name})
endforeach (l) 

include_directories(${LLVM_INCLUDES})
link_directories(${LLVM_LIBRARIES})
add_subdirectory(src)

# TODO: tentative tests. fix
add_test(test-ary   "./tourmaline" "example/ary.rb")
add_test(test-fact  "./tourmaline" "example/fact.rb")
add_test(test-hello "./tourmaline" "example/hello.rb")
add_test(test-if    "./tourmaline" "example/if.rb")
add_test(test-op    "./tourmaline" "example/op.rb")
add_test(test-var   "./tourmaline" "example/var.rb")
