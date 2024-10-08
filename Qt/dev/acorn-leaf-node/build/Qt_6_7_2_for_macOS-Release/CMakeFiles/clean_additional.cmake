# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/acorn-leaf-node_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/acorn-leaf-node_autogen.dir/ParseCache.txt"
  "acorn-leaf-node_autogen"
  )
endif()
