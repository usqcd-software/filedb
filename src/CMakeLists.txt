set( FILEDB_SRCS
	FileDB.cpp
	DBCursor.cpp
	DBFunc.cpp
	DBString.cpp
	ConfDataStoreDB.cpp
	AllConfStoreDB.cpp
	ConfDataDBMerger.cpp
	ConfigInfo.cpp
	HashFunc.cpp
	VFloatData.cpp
)

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
	

add_library(filedb ${FILEDB_SRCS})
if( FileDB_BIG_ENDIAN )
  target_compile_definitions(filedb PUBLIC -D_FILEDB_BIG_ENDIAN)
else()
  target_compile_definitions(filedb PUBLIC -D_FILEDB_LITTLE_ENDIAN)
endif()

if( FileDB_ENABLE_TEMPLATE_COMPILE_TEST )
  target_compile_definitions(filedb PUBLIC -D_FILEDB_COMPILE_TEST)
endif()

target_include_directories( filedb PUBLIC
						   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
						   $<INSTALL_INTERFACE:include> )
						   
target_include_directories( filedb PUBLIC
						   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
						   $<INSTALL_INTERFACE:include> )

target_link_libraries( filedb filehash ) 
set_target_properties(filedb PROPERTIES CXX_STANDARD 14)
set_target_properties(filedb PROPERTIES CXX_EXTENSIONS OFF)

macro(add_test_cpp EXECNAME)
    # Make the executable
    add_executable(${EXECNAME} ${EXECNAME}.cpp)
	set_target_properties(${EXECNAME} PROPERTIES CXX_STANDARD 14)
	set_target_properties(${EXECNAME} PROPERTIES CXX_EXTENSIONS OFF)
    target_link_libraries(${EXECNAME} filedb )
	install( TARGETS ${EXECNAME} DESTINATION bin )
endmacro()

add_test_cpp(ConfCreateTest)
add_test_cpp(SConfCreateTest)
add_test_cpp(SConfReadTest)
add_test_cpp(ConfReadTest)
add_test_cpp(AllConfCreateTest)
add_test_cpp(AllConfReadTest)
add_test_cpp(AllConfUpdateTest)
add_test_cpp(DBMergeTest)

install( FILES ${FILEDB_HEADERS} DESTINATION include)
install( TARGETS filedb
		 EXPORT filedb 
		 ARCHIVE DESTINATION lib 
		 LIBRARY DESTINATION lib
		 RUNTIME DESTINATION bin)
		 

	