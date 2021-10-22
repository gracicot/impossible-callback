cmake_minimum_required(VERSION 3.21)

project(impossible-callback
	VERSION 0.0.1
	DESCRIPTION "100% meatless callback"
	LANGUAGES CXX
)

option(IMPOSSIBLE_CALLBACK_BUILD_TESTS "Enable tests" OFF)
option(IMPOSSIBLE_CALLBACK_BUILD_BENCHMARKS "Enable benchmarks" OFF)
option(IMPOSSIBLE_CALLBACK_BUILD_EXAMPLES "Enable examples" OFF)

if(IMPOSSIBLE_CALLBACK_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

add_library(impossible-callback INTERFACE)
add_library(impossible-callback::impossible-callback ALIAS impossible-callback)

# For CMAKE_INSTALL_<LIBDIR, INLCUDEDIR, BINDIR, DATADIR>
include(GNUInstallDirs)

target_include_directories(impossible-callback INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(impossible-callback INTERFACE cxx_std_17)

install(
  DIRECTORY include/impossible-callback
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.hpp"
)

install(TARGETS impossible-callback
  EXPORT impossible-callback-config
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

export(
  TARGETS impossible-callback
  NAMESPACE impossible-callback::
  FILE "${PROJECT_BINARY_DIR}/impossible-callback-config.cmake"
)

install(EXPORT impossible-callback-config
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/impossible-callback"
  NAMESPACE impossible-callback::
)
