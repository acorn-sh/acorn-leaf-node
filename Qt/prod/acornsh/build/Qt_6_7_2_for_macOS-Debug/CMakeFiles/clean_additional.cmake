# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/acornsh_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/acornsh_autogen.dir/ParseCache.txt"
  "acornsh_autogen"
  )
endif()
