cmake_minimum_required(VERSION 3.17)
project(FileDB VERSION 1.2
              DESCRIPTION "File DB"
              LANGUAGES CXX;C )


# Basic Includes
# Helps with installig
include(GNUInstallDirs)

# Can run CTest Tests if any
include(CTest)

# Helps with Exporting and Creating XXXConfig.cmake scripts
include(CMakePackageConfigHelpers)


# Set yes/no options 
# OFF is default value
option(FileDB_ENABLE_DEBUG_HASHDB "Turn on low level debug verbose messages." OFF)
option(FileDB_ENABLE_TEMPLATE_COMPILE_TEST "Test compile template headers through dumb instantiations." OFF)
# Enable Address address and undefined behaviour sanitizers
option(FileDB_ENABLE_SANITIZERS "Enable Address and Undefined Behaviour Sanitizers" OFF)

# Get Pthread options
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


# Check for include files -- I am using the set from the filedb_config_internal.h.in file
include(CheckIncludeFile)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(stdint.h HAVE_STDINT_H)
check_include_file(stdlib.h HAVE_STDLIB_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(unistd.h HAVE_UNISTD_H)

# Set appropriate variables in generated header
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

# Deal with Sanitizer
if( FileDB_ENABLE_SANITIZERS )
  if( ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 3.1)
     OR ( CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.8) )
      message(STATUS "FileDB: Setting Sanitizer options")
      set(FileDB_SANITIZER_OPTS "-fsanitize=address,undefined")
  else()
    message(STATUS "Don't know how to enable sanitizers for ${CMAKE_CXX_COMPILER_ID}... Ignoring")
  endif()
endif() 

# Configure Filehash
add_subdirectory(filehash)

# Configure src/
add_subdirectory(src)

#install generated header file:
install( FILES ${CMAKE_CURRENT_BINARY_DIR}/src/filedb_config_internal.h DESTINATION include )

#Install the filedb.cmake file which contains the exports for importing
install(EXPORT FileDBTargets NAMESPACE FileDB:: DESTINATION lib/cmake/FileDB )

#Create a packageConfig.cmake file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/FileDBConfig.cmake"
  INSTALL_DESTINATION "lib/cmake/FileDB"
)

# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/FileDBConfigVersion.cmake"
  VERSION "${FileDB_VERSION_MAJOR}.${FileDB_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

# install it
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/FileDBConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/FileDBConfigVersion.cmake
  DESTINATION lib/cmake/FileDB
)

# export package. Does nothing by default, but
# can have uses in specially set up configurations
export(PACKAGE FileDB)

