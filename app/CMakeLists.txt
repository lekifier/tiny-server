file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_executable(app ${srcs})
target_include_directories(app PUBLIC include)

target_link_libraries(app PUBLIC tinyServerCore)