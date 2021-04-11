cmake_minimum_required(VERSION 3.0.0)
project(search_engine VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

set(SRC)

#######################################
# MAIN
#######################################
add_executable(main src/main.cpp ${SRC})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})


#######################################
# GOOGLE TESTS
#######################################
find_package(GTest REQUIRED)
include_directories(src tests ${GTEST_INCLUDE_DIRS})
add_executable(gtest-single_linked_list tests/single_linked_list.cpp ${SRC})


#######################################
# UNIT TESTS
#######################################
enable_testing()

target_link_libraries(gtest-single_linked_list GTest::Main ${GTEST_LIBRARIES})
add_test(NAME single_linked_list COMMAND gtest-single_linked_list)
