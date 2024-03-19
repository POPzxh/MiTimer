# 配置编译类型
set(CMAKE_BUILD_TYPE "Debug")

cmake_minimum_required(VERSION 3.15)

# 设置项目版本和生成version.h
project(TimerQueue VERSION 1.0.0 LANGUAGES C CXX)

# 指定编程语言版本
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# 配置编译选项
add_compile_options(-Wall -Wextra -pedantic -Werror)

# 根据编译类型设置编译选项
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# 添加全局宏定义
# add_definitions(-DTEST)

find_package(Threads)
if (Threads_FOUND)
    message("Find Threads library ${Threads_FOUND}: ${Threads_INCLUDE_DIR}, ${Threads_LIBRARYS}")
endif()

include_directories(/usr/local/ocilib/include)
link_directories(/usr/local/ocilib/lib)

# 生成可执行文件
add_executable(queueMain src/main.cpp)

add_library(MiTimer lib/MiTimer.cpp)
add_executable(main main.c)

target_link_libraries(queueMain PRIVATE 
    pthread
    les::TimerQueue
    ocilib
)

target_link_libraries(
    MiTimer PRIVATE
    les::TimerQueue
)

target_link_libraries(
    main PRIVATE
    MiTimer
    pthread
)

# 添加测试
enable_testing()

# 添加子目录
add_subdirectory(src)
add_subdirectory(tests)
# message(${CMAKE_SOURCE_DIR})
# install(TARGETS queueMain DESTINATION ~/work/TimerQueue/output)
# install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/ 
#         DESTINATION ${CMAKE_SOURCE_DIR}/output
#         FILES_MATCHING PATTERN "*.h" )
