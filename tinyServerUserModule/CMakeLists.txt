file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_library(tinyServerUserModule STATIC ${srcs})

target_include_directories(tinyServerUserModule PUBLIC include)
target_link_libraries(tinyServerUserModule PUBLIC tinyServerCore)
