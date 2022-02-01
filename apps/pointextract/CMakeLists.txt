find_package(cxxopts)
find_package(spdlog)
find_package(tinyply)

add_executable(pointextract main.cpp)
target_link_libraries(pointextract PRIVATE mvaformat 
        cxxopts::cxxopts spdlog::spdlog tinyply::tinyply)

install(TARGETS pointextract
        RUNTIME DESTINATION bin)