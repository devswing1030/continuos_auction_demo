cmake_minimum_required(VERSION 3.23)
project(auction)

#set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_STANDARD 17)

set(GOOGLETEST_VERSION "1.15.2")

include_directories(${PROJECT_SOURCE_DIR}/src)
# 添加 Google Test 子目录
#add_subdirectory(3rd/googletest/googletest)
add_subdirectory(3rd/googletest/googlemock)

# 包含 Google Test 头文件
include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${gmock_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd)

#add_executable(auction main.cpp Order.h OrderQueue.h OrderBook.h ContinuosAuction.h
#        Engine.h Engine.cpp AuctionProcessor.h AuctionProcessor.cpp SecuritySession.h   SecuritySession.cpp)

# 获取 tests 目录下的所有 .cpp 文件
file(GLOB TEST_SOURCES test/*.cpp)
file(GLOB PROJECT_SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# 遍历每个测试文件，生成可执行文件并链接
foreach(test_source ${TEST_SOURCES})
    # 获取测试文件名，不带路径和扩展名
    get_filename_component(test_name ${test_source} NAME_WE)

    # 为每个测试文件创建一个可执行文件
    add_executable(${test_name} ${test_source} ${PROJECT_SOURCES})

    # 链接测试文件到目标库和 gtest 库
    target_link_libraries(${test_name} gtest gtest_main)

    # 注册测试到 CTest
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# 添加测试
enable_testing()


