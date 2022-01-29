################
## Conan.io
################
# Download automatically, you can also just copy the conan.cmake file
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
            "${CMAKE_BINARY_DIR}/conan.cmake")
endif ()

include(${CMAKE_BINARY_DIR}/conan.cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CONAN_PACKAGES 
        eigen/3.3.9
        zstd/1.5.0
        zlib/1.2.11
)
set(CONAN_CMAKE_OPTIONS "")

if(BUILD_TESTS)
        list(APPEND CONAN_PACKAGES doctest/2.4.8)
endif()

conan_cmake_configure(
        REQUIRES
        ${CONAN_PACKAGES}
        GENERATORS cmake_find_package
        OPTIONS ${CONAN_CMAKE_OPTIONS}
)

conan_cmake_autodetect(settings
        BUILD_TYPE "Release")

conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        REMOTE conancenter
        SETTINGS ${settings})