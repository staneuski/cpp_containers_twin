cmake_minimum_required(VERSION 3.0.0)
project(custom_stl VERSION 0.1.3)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS_DEBUG -fsanitize=address)

add_compile_options(-Wall -Wextra -Werror)

include_directories(src tests)

set(OPTIONAL)
set(SIMPLE_VECTOR)
set(SINGLE_LINKED_LIST)
set(VECTOR)

set(SRC ${SIMPLE_VECTOR} ${OPTIONAL} ${SINGLE_LINKED_LIST} ${VECTOR})


#######################################
# APPS
#######################################
add_executable(main src/main.cpp ${SRC})


#######################################
# TESTS
#######################################
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/0d9daa99365eb00a2b81392b6ec2c8a802769ef0.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

#- src/matrix
# add_executable(gtest-matrix tests/g-matrix.cpp ${OPTIONAL})
# target_link_libraries(gtest-matrix gtest_main)
# add_test(NAME matrix COMMAND gtest-matrix)

#- src/optional
add_executable(gtest-optional tests/g-optional.cpp ${OPTIONAL})
target_link_libraries(gtest-optional gtest_main)
add_test(NAME optional COMMAND gtest-optional)

#- src/simple_vector
add_executable(gtest-simple_vector tests/g-simple_vector.cpp ${SIMPLE_VECTOR})
target_link_libraries(gtest-simple_vector gtest_main)
add_test(NAME simple_vector COMMAND gtest-simple_vector)

#- src/single_linked_list
add_executable(gtest-single_linked_list tests/g-single_linked_list.cpp ${SINGLE_LINKED_LIST})
target_link_libraries(gtest-single_linked_list gtest_main)
add_test(NAME single_linked_list COMMAND gtest-single_linked_list)
