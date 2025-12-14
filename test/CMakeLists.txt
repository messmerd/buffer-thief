

# TODO: Use CMake macro to build tests with choice of C++ standard and standard library

###############################################

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(INSTALL_GTEST OFF)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

###############################################

add_executable(StringTest string_test.cc)
target_link_libraries(StringTest PRIVATE messmerd::bufferthief GTest::gtest_main)
target_compile_features(StringTest PRIVATE cxx_std_20)

###############################################

include(GoogleTest)
gtest_discover_tests(StringTest)
