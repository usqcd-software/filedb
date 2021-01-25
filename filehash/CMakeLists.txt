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

target_compile_definitions(filehash PRIVATE _FILE_OFFSET_BITS=64)
if( FileDB_ENABLE_DEBUG_HASHDB ) 
  target_compile_definitions(filehash PUBLIC -D_FFDB_DEBUG)
endif()


target_include_directories( filehash PUBLIC
						   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
						   $<INSTALL_INTERFACE:include> )

target_link_libraries( filehash PUBLIC Threads::Threads)

macro(add_test EXECNAME)
    # Make the executable
    add_executable(${EXECNAME} ${EXECNAME}.c)

    target_link_libraries(${EXECNAME} filehash )
	install( TARGETS ${EXECNAME} DESTINATION bin )
endmacro()

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
install( TARGETS filehash
		 EXPORT filedb
		 ARCHIVE DESTINATION lib 
		 LIBRARY DESTINATION lib
		 RUNTIME DESTINATION bin)
