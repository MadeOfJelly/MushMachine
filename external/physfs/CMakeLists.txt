cmake_minimum_required(VERSION 3.9...3.16 FATAL_ERROR)

set(PHYSFS_BUILD_SHARED FALSE CACHE INTERNAL "")
set(PHYSFS_BUILD_TEST FALSE CACHE INTERNAL "")
add_subdirectory("physfs")

