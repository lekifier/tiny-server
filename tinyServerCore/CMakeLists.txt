file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_library(tinyServerCore STATIC ${srcs})

find_package(Threads REQUIRED)
target_include_directories(tinyServerCore PUBLIC include)
target_link_libraries(tinyServerCore PRIVATE Threads::Threads)
