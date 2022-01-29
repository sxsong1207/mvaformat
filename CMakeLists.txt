cmake_minimum_required(VERSION 3.2)
project(mvaformat)
set(CMAKE_PROJECT_VERSION_MAJOR 2)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 0)
set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION_MAJOR}.${CMAKE_PROJECT_VERSION_MINOR}.${CMAKE_PROJECT_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 17)
option(WITH_OPENMVS "build with openmvs" OFF)
option(BUILD_SHARED "build shared library" OFF)
option(BUILD_TESTS "build tests" OFF)
option(BUILD_C_MODULES "build c++ modules" OFF)
option(BUILD_PYTHON_MODULES "build python binding" OFF)

include(cmake/deps.cmake) # Install External libs with conan

find_package(ZLIB REQUIRED)
find_package(zstd REQUIRED)
find_package(Eigen3 REQUIRED)

if(BUILD_C_MODULES)
    add_subdirectory(src)
endif()
if(BUILD_PYTHON_MODULES)
    find_package(OpenMP)
    if(OpenMP_FOUND)
        add_definitions(-D_USE_OPENMP)
    endif()
    find_package(pybind11 CONFIG REQUIRED)
    add_subdirectory(pybind11)
endif()

if(BUILD_TESTS)
    find_package(doctest REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE mvaformat)

if(WIN32 AND NOT CYGWIN)
    set(INSTALL_CMAKE_DIR "CMake")
else()
    set(INSTALL_CMAKE_DIR "lib/cmake")
endif()

# Install the export set for use with the install-tree
install(EXPORT mvaformatTargets
	NAMESPACE mvaformat::
	DESTINATION "${INSTALL_CMAKE_DIR}")

include(CMakePackageConfigHelpers)
set(INSTALL_CMAKE_DIR_IN ${INSTALL_CMAKE_DIR})
set(INSTALL_INCLUDE_DIR_IN "include/mvaformat")
set(MVAFORMAT_DEFINITIONS "-D_USE_EIGEN -D_USE_GZSTREAM -D_USE_ZSTDSTREAM")
configure_package_config_file(mvaformatConfig.cmake.in
  ${PROJECT_BINARY_DIR}/mvaformatConfig.cmake
  INSTALL_DESTINATION "${INSTALL_CMAKE_DIR}"
  NO_SET_AND_CHECK_MACRO)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/mvaformatConfigVersion.cmake
  VERSION ${CMAKE_PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mvaformatConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/mvaformatConfigVersion.cmake
        DESTINATION "${INSTALL_CMAKE_DIR}" )