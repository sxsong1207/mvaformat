include_directories(${CMAKE_SOURCE_DIR}/external)
include_directories(${CMAKE_SOURCE_DIR}/src)

set(SRCS pymvaformat_pybind11.cpp
         pymvaformat_pybind11_scene.cpp
         ${CMAKE_SOURCE_DIR}/external/zstdstream/zstdstream.cpp)

set(HEADERS pymvaformat_pybind11_scene.h)

pybind11_add_module(pymvaformat ${SRCS})
target_include_directories(pymvaformat PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pymvaformat PRIVATE zstd::zstd ZLIB::ZLIB Eigen3::Eigen OpenMP::OpenMP_CXX)
# target_compile_definitions(pymvaformat PRIVATE MVAFORMAT_API_EXPORTS)

install(TARGETS pymvaformat
        EXPORT mvaformatTargets
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)