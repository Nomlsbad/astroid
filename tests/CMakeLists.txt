add_executable(${TEST_NAME} ${TEST_SOURCES})

target_include_directories(${TEST_NAME}
        PUBLIC
        ${APP_INCLUDE_PATH}
)

target_link_libraries(${TEST_NAME}
        PRIVATE
        GTest::gtest
        GTest::gtest_main
        ${APP_NAME}
)

include(GoogleTest)
gtest_discover_tests(${TEST_NAME})

add_test(NAME ${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${TEST_NAME} "--gtest_color=yes"
)