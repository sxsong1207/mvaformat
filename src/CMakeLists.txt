include_directories(${CMAKE_SOURCE_DIR}/external)

set(SRCS mvaformat.cpp
         ${CMAKE_SOURCE_DIR}/external/zstdstream/zstdstream.cpp)

set(HEADERS
        config.h
        mvaformat.h)

if(BUILD_SHARED)
        add_definitions(-DMVAFORMAT_SHARED)
        add_library(mvaformat SHARED ${SRCS})
else()
        add_definitions(-DMVAFORMAT_STATIC)
        add_library(mvaformat STATIC ${SRCS})
endif()

target_link_libraries(mvaformat PUBLIC Eigen3::Eigen PRIVATE zstd::zstd ZLIB::ZLIB )
set_target_properties(mvaformat PROPERTIES PUBLIC_HEADER "${HEADERS}")
if(BUILD_SHARED)
        target_compile_definitions(mvaformat PRIVATE MVAFORMAT_API_EXPORTS)
endif()

install(TARGETS mvaformat
        EXPORT mvaformatTargets
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/mvaformat)


install(FILES mvaformat_impl.hpp
        DESTINATION include/mvaformat/impl)
        

