cmake_minimum_required(VERSION 3.15)
project(FileDB VERSION 1.2
              DESCRIPTION "File DB"
              LANGUAGES CXX;C )


# Basic Includes
include(GNUInstallDirs)
include(CTest)
include(CMakePackageConfigHelpers)
include(CheckIncludeFile)

# Set yes/no options 
# OFF is default value
option(FileDB_ENABLE_DEBUG_HASHDB "Turn on low level debug verbose messages." OFF)
option(FileDB_ENABLE_TEMPLATE_COMPILE_TEST "Test compile template headers through dumb instantiations." OFF)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Check files
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(unistd.h HAVE_UNISTD_H)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/filedb_config_internal.h.cmake.in
			   ${CMAKE_CURRENT_BINARY_DIR}/src/filedb_config_internal.h)
			   
# Check endianness
include(TestBigEndian)
TEST_BIG_ENDIAN(FileDB_BIG_ENDIAN)
if( FileDB_BIG_ENDIAN ) 
  message(STATUS "System is big endian")
else()
  message(STATUS "System is little endian")
endif()

# Check include file

# Set C++ standard to 14

add_subdirectory(filehash)
add_subdirectory(src)

#install generated header file:
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/src/filedb_config_internal.h DESTINATION include )

install(EXPORT filedb NAMESPACE filedb:: DESTINATION lib/cmake/filedb )

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/filedbConfig.cmake"
  INSTALL_DESTINATION "lib/cmake/filedb"
)

# install the configuration file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/filedbConfig.cmake
  DESTINATION lib/cmake/filedb
)

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/filedbConfigVersion.cmake"
  VERSION "${FileDB_VERSION_MAJOR}.${FileDB_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

# install the configuration file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/filedbConfigVersion.cmake
  DESTINATION lib/cmake/filedb
)


