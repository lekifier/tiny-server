add_executable(server src/server/server.cpp)
target_include_directories(server PUBLIC include)
target_link_libraries(server PUBLIC tinyServerCore)

add_executable(client src/client/client.cpp)