cmake_minimum_required(VERSION 2.8)
project(tourmaline)

add_executable(tourmaline 
  ast.cpp
  codegen.cpp
  error.cpp
  func.cpp
  lexer.cpp
  main.cpp
  parser.cpp
  tourmaline.cpp
  type.cpp
  var.cpp
)
target_link_libraries(tourmaline ${LLVM_LIBS_ALL})
