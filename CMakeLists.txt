cmake_minimum_required(VERSION 3.23...4.0)

project(
	BufferThief
	VERSION 0.1
	DESCRIPTION "Small library for stealing the internal buffers of standard C++ containers and strings"
	LANGUAGES CXX
)

option(
	BT_USE_UNSTABLE_STEAL
	"Use standard library implementation details to steal internal buffers rather than simply copying them"
	ON
)

option(BT_BUILD_TESTS "Build BufferThief unit tests" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(bufferthief INTERFACE)
add_library(messmerd::bufferthief ALIAS bufferthief)

target_compile_features(bufferthief INTERFACE cxx_std_17)

configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/include/bufferthief/config.hh.in"
	"${CMAKE_CURRENT_BINARY_DIR}/include/bufferthief/config.hh"
)

target_sources(
	bufferthief
	INTERFACE
	FILE_SET HEADERS
	BASE_DIRS
		${CMAKE_CURRENT_SOURCE_DIR}/include
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
	FILES
		include/bufferthief/private/member_accessor.hh
		include/bufferthief/private/string_libstdc++.hh
		include/bufferthief/private/string_msvc_stl.hh
		include/bufferthief/string.hh
		include/bufferthief/vector.hh
		${CMAKE_CURRENT_BINARY_DIR}/include/bufferthief/config.hh
)

install(
	TARGETS bufferthief
	EXPORT bufferthiefTargets
	FILE_SET HEADERS
)

install(
	EXPORT bufferthiefTargets
	NAMESPACE messmerd::
	DESTINATION lib/cmake/bufferthief
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/bufferthiefConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/bufferthiefConfig.cmake
	INSTALL_DESTINATION lib/cmake/bufferthief
)

install(FILES
	${CMAKE_CURRENT_BINARY_DIR}/bufferthiefConfig.cmake
	DESTINATION lib/cmake/bufferthief
)

if(BT_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()
