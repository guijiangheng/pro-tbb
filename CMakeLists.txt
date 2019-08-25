cmake_minimum_required(VERSION 3.12)
project(protbb LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(TBB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ext/tbb)
include(${TBB_ROOT}/cmake/TBBBuild.cmake)
tbb_build(TBB_ROOT ${TBB_ROOT} CONFIG_DIR TBB_DIR MAKE_ARGS tbb_cpf=1)

find_package(TBB REQUIRED tbb_preview)

add_executable(protbb src/main.cpp)
target_compile_features(protbb PRIVATE cxx_std_17)
target_link_libraries(protbb PRIVATE ${TBB_IMPORTED_TARGETS})
target_include_directories(protbb
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ext/pstl/include
          ${CMAKE_CURRENT_SOURCE_DIR}/include
)
