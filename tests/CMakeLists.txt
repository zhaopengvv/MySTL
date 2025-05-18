set(TEST_SOURCES
    construct_test.cpp
)

add_executable(mystl_test ${TEST_SOURCES})
target_link_libraries(mystl_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(mystl_test) 