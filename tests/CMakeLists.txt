set(CppUTest_DIR ${PROJECT_SOURCE_DIR}/deps/cpputest)
include_directories(${CppUTest_DIR}/include)
link_directories(${CppUTest_DIR}/build/src/CppUTest)
link_directories(${CppUTest_DIR}/build/src/CppUTestExt)

file(GLOB SOURCES *.cpp)

add_executable(unit ${SOURCES})
target_link_libraries(unit src CppUTest CppUTestExt pthread)

add_custom_target(
    unit-tests
    COMMAND ${PROJECT_BINARY_DIR}/tests/unit
    DEPENDS unit)
