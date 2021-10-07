add_library(filedb 
        FileDB.cpp
        FileDB.h
        Serializable.h
        DBKey.h
        DBData.h
        DBCursor.h
        DBCursor.cpp
        DBFunc.cpp
        DBFunc.h
        DBString.cpp
        DBString.h
        ConfDataStoreDB.cpp
        ConfDataStoreDB.h
        ConfDataStoreMultipleDB.h
        AllConfStoreDB.h
        AllConfStoreDB.cpp
        AllConfStoreMultipleDB.h
        ConfDataDBMerger.h
        ConfDataDBMerger.cpp
        ConfigInfo.h
        ConfigInfo.cpp
        HashFunc.h
        HashFunc.cpp
        VFloatData.h
        VFloatData.cpp
)

# Collect the headers for installing (follow Makefile.am)
set(FILEDB_HEADERS 
	filedb_config.h
	FileDB.h
	Serializable.h
	DBKey.h
	DBData.h
	DBCursor.h
	DBFunc.h
	DBString.h
	ConfDataStoreDB.h
	ConfDataStoreMultipleDB.h
	AllConfStoreDB.h
	AllConfStoreMultipleDB.h
	ConfDataDBMerger.h
	ConfigInfo.h
	HashFunc.h
)
	
# Add appropriate Endianness option
if( FileDB_BIG_ENDIAN )
  target_compile_definitions(filedb PUBLIC -D_FILEDB_BIG_ENDIAN)
else()
  target_compile_definitions(filedb PUBLIC -D_FILEDB_LITTLE_ENDIAN)
endif()

# Add option for compile testing
if( FileDB_ENABLE_TEMPLATE_COMPILE_TEST )
  target_compile_definitions(filedb PUBLIC -D_FILEDB_COMPILE_TEST)
endif()

# Add include directories
target_include_directories( filedb PUBLIC
						   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
						   $<INSTALL_INTERFACE:include> )
						 
# Allow to include the built sources  
target_include_directories( filedb PUBLIC
						   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
						   $<INSTALL_INTERFACE:include> )

# Link the filehash library to this one
target_link_libraries( filedb filehash )

# Set minimum C++ standard to C++14 
set_target_properties(filedb PROPERTIES CXX_STANDARD 14)

# Disallow extensions to the standard
set_target_properties(filedb PROPERTIES CXX_EXTENSIONS OFF)

if( FileDB_SANITIZER_OPTS )
  target_compile_options(filedb PRIVATE  ${FileDB_SANITIZER_OPTS})
  target_link_options(filedb PUBLIC ${FileDB_SANITIZER_OPTS})
endif() 

# Macro to build the exeictables
macro(add_test_cpp EXECNAME)
    # Make the executable
    add_executable(${EXECNAME} ${EXECNAME}.cpp)
    
    # Set C++ Standard stuff
	set_target_properties(${EXECNAME} PROPERTIES CXX_STANDARD 14)
	set_target_properties(${EXECNAME} PROPERTIES CXX_EXTENSIONS OFF)
	
	# Link filedb (and its dependencies)
    target_link_libraries(${EXECNAME} filedb )
    
    # Install result into bin/
	install( TARGETS ${EXECNAME} DESTINATION bin )
endmacro()

# Build the executables
add_test_cpp(ConfCreateTest)
add_test_cpp(SConfCreateTest)
add_test_cpp(SConfReadTest)
add_test_cpp(ConfReadTest)
add_test_cpp(AllConfCreateTest)
add_test_cpp(AllConfReadTest)
add_test_cpp(AllConfUpdateTest)
add_test_cpp(DBMergeTest)

# Install the headers
install( FILES ${FILEDB_HEADERS} DESTINATION include)

# Install filedb into the target
install( TARGETS filedb
		 EXPORT FileDBTargets
		 ARCHIVE DESTINATION lib 
		 LIBRARY DESTINATION lib
		 RUNTIME DESTINATION bin)
		 
add_library(FileDB::filedb ALIAS filedb)
	
