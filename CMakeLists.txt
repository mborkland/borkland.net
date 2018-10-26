cmake_minimum_required(VERSION 3.10)
project(source_files)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/clang++.exe")
set(CLANG++_COMPILER_FLAGS "-Wall -Werror -pedantic -Wconversion")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

include_directories(algorithms/sorting/heapsort)
include_directories(algorithms/sorting/insertion-sort)
include_directories(algorithms/sorting/merge-sort)
include_directories(algorithms/sorting/quicksort/hoare)
include_directories(algorithms/sorting/quicksort/lomuto)
include_directories(algorithms/sorting/quicksort/random)
include_directories(algorithms/sorting/selection-sort)
include_directories(algorithms/string/LCS)
include_directories(data-structures/src)

set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/catch)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

set(CATCH_OBJECT_FILE data-structures/test/tests-main.o)
set(DS_TEST_DIR data-structures/test)

set(DLINKEDLIST_SOURCE_FILES ${DS_TEST_DIR}/tests-DLinkedList.cpp ${CATCH_OBJECT_FILE})
add_executable(tests-DLinkedList ${DLINKEDLIST_SOURCE_FILES})
target_link_libraries(tests-DLinkedList Catch)

set(SLINKEDLIST_SOURCE_FILES ${DS_TEST_DIR}/tests-SLinkedList.cpp ${CATCH_OBJECT_FILE})
add_executable(tests-SLinkedList ${SLINKEDLIST_SOURCE_FILES})
target_link_libraries(tests-SLinkedList Catch)

set(AVLTREE_SOURCE_FILES ${DS_TEST_DIR}/tests-AVLTree.cpp ${CATCH_OBJECT_FILE})
add_executable(tests-AVLTree ${AVLTREE_SOURCE_FILES})

set(REDBLACKTREE_SOURCE_FILES ${DS_TEST_DIR}/tests-RedBlackTree.cpp ${CATCH_OBJECT_FILE})
add_executable(tests-RedBlackTree ${REDBLACKTREE_SOURCE_FILES})