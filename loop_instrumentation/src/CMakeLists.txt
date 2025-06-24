cmake_minimum_required(VERSION 3.20)
project(LoopWrapperPass)

find_package(LLVM 15.0 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_CXXFLAGS} -fno-rtti")

add_library(LoopWrapperPass MODULE
  LoopWrapperPass.cpp
)

target_link_libraries(LoopWrapperPass PRIVATE
  LLVMCore
  LLVMAnalysis
  LLVMSupport
  LLVMTransformUtils
  LLVMPasses
)