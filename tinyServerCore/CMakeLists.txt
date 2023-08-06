file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/*.cpp include/*.hpp)
add_library(tinyServerCore STATIC ${srcs})

target_include_directories(tinyServerCore PUBLIC include)
