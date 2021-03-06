# SET (CMAKE_C_COMPILER             "/usr/bin/clang")
# SET (CMAKE_C_FLAGS                "-Wall -std=c99")
# SET (CMAKE_C_FLAGS_DEBUG          "-g")
# SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
# SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
# SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")
#
# SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
# SET (CMAKE_CXX_FLAGS                "-Wall")
# SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
# SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
# SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
# SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# SET (CMAKE_AR      "/usr/bin/llvm-ar")
# SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
# SET (CMAKE_NM      "/usr/bin/llvm-nm")
# SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
# SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")


cmake_minimum_required(VERSION 3.6)
project(util)
set(EXECUTABLE_OUTPUT_PATH "bin")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++")
set(SOURCE_FILES asset.cpp dag.cpp)
include_directories(/usr/local/include/graphviz)
link_directories(/usr/local/lib)
link_directories(/usr/local/lib/graphviz)
add_library(lib STATIC ${SOURCE_FILES})
add_executable (util main.cpp)
target_link_libraries(util lib gvpr gvc cgraph gvplugin_core gvplugin_dot_layout )
add_custom_command( TARGET util
  COMMAND echo "copy static files"
  COMMAND cp ${PROJECT_SOURCE_DIR}/index.html ${CMAKE_BINARY_DIR}/bin/index.html
  COMMAND cp ${PROJECT_SOURCE_DIR}/resource.txt ${CMAKE_BINARY_DIR}/bin/resource.txt
  )
