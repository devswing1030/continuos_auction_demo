cmake_minimum_required(VERSION 3.23)
project(auction)

set(CMAKE_CXX_STANDARD 17)

set(GOOGLETEST_VERSION "1.15.2")

include_directories(${PROJECT_SOURCE_DIR})
# 添加 Google Test 子目录
#add_subdirectory(3rd/googletest/googletest)
add_subdirectory(3rd/googletest/googlemock)

# 包含 Google Test 头文件
include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gmock_SOURCE_DIR}/include)

add_executable(auction main.cpp Order.h OrderQueue.h OrderBook.h ContinuosAuction.h)

# 获取 tests 目录下的所有 .cpp 文件
file(GLOB TEST_SOURCES test/*.cpp)

# 遍历每个测试文件，生成可执行文件并链接
foreach(test_source ${TEST_SOURCES})
    # 获取测试文件名，不带路径和扩展名
    get_filename_component(test_name ${test_source} NAME_WE)

    # 为每个测试文件创建一个可执行文件
    add_executable(${test_name} ${test_source})

    # 链接测试文件到目标库和 gtest 库
    target_link_libraries(${test_name} gtest gtest_main)

    # 注册测试到 CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# 添加测试
enable_testing()


