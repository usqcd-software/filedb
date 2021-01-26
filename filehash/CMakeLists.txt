# Create the library and from sources
add_library(filehash
	ffdb_cq.h
	ffdb_db.h
	ffdb_db.c
	ffdb_hash.h
	ffdb_hash.c
	ffdb_hash_func.h
	ffdb_hash_func.c
	ffdb_page.h
	ffdb_page.c
	ffdb_pagepool.h
	ffdb_pagepool.c)

# Use _FILE_OFFSET_BITS=64 just to build this. However there should be
# a good way to determine if this is needed for large files or not
target_compile_definitions(filehash PRIVATE _FILE_OFFSET_BITS=64)

# If FileDB_ENABLE_DEBUG_HASHDB was set, enable the compile definition
if( FileDB_ENABLE_DEBUG_HASHDB ) 
  target_compile_definitions(filehash PUBLIC -D_FFDB_DEBUG)
endif()

# Set the include dirs, to the current dir (for building) and include/ for 
# post install
target_include_directories( filehash PUBLIC
						   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
						   $<INSTALL_INTERFACE:include> )

# Link against the thread libs
target_link_libraries( filehash PUBLIC Threads::Threads)

# Define a macro to build our executables
macro(add_test EXECNAME)
	# Add the executable
    add_executable(${EXECNAME} ${EXECNAME}.c)

	# Link against filehash
    target_link_libraries(${EXECNAME} filehash )
    
    # Install the executable in bin/
	install( TARGETS ${EXECNAME} DESTINATION bin )
endmacro()

# Create all the executables
add_test(tcreate)
add_test(twrite)
add_test(tdualwrite)
add_test(treplace)
add_test(tread)
add_test(tread-th)
add_test(tkeyseq)
add_test(tcheck)
add_test(createStrings)
add_test(replaceStrings)


# Install the public header
install( FILES ffdb_db.h DESTINATION include)

# write the target into the filedb export 
install( TARGETS filehash
		 EXPORT filedb
		 ARCHIVE DESTINATION lib 
		 LIBRARY DESTINATION lib
		 RUNTIME DESTINATION bin)
