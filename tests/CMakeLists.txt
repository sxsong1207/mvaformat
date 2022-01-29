include_directories(${CMAKE_SOURCE_DIR}/src)

if(BUILD_C_MODULES)
add_executable(test_readtestdata test_readtestdata.cpp)
target_link_libraries(test_readtestdata mvaformat doctest::doctest)

add_test(NAME test_readtestdata
        COMMAND test_readtestdata --testdatadir ${CMAKE_CURRENT_SOURCE_DIR}/testdata)
endif(BUILD_C_MODULES)