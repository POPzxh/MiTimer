find_package(GTest)

add_executable(mytest gtest_main.cpp)

target_link_libraries(mytest PRIVATE 
    GTest::GTest
    les::TimerQueue
)


add_test(test mytest)