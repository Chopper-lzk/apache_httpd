cmake_minimum_required(VERSION 3.20)
project(httpserver)

include_directories(include)
include_directories(/usr/local/include)

aux_source_directory(src source_list)
aux_source_directory(src/os/unix os_source_list)

set(APR_DYNAMIC_DIR /usr/local/lib/lib/libapr-2.dylib)

add_executable(httpserver  ${source_list} ${os_source_list} )
target_link_libraries(httpserver ${APR_DYNAMIC_DIR})