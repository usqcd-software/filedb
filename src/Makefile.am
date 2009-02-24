AM_CXXFLAGS = @FFDB_BYTE_ORDER_DEF@ -I@top_srcdir@/filehash

lib_LIBRARIES = libffdbfile.a

libffdbfile_a_SOURCES = \
	FFDB.cpp \
	FFDB.h \
	Serializable.h \
	DBKey.h \
	DBData.h \
	DBCursor.h \
	DBCursor.cpp \
	DBFunc.cpp \
	DBFunc.h \
	DBString.cpp \
	DBString.h \
	ConfDataStoreDB.cpp \
	ConfDataStoreDB.h \
	AllConfStoreDB.h \
	AllConfStoreDB.cpp \
	ConfDataDBMerger.h \
	ConfDataDBMerger.cpp \
	ConfigInfo.h \
	ConfigInfo.cpp \
	HashFunc.h \
	HashFunc.cpp

include_HEADERS = \
	FFDB.h \
	Serializable.h \
	DBKey.h \
	DBData.h \
	DBCursor.h \
	DBFunc.h \
	DBString.h \
	ConfDataStoreDB.h \
	AllConfStoreDB.h \
	ConfDataDBMerger.h \
	ConfigInfo.h \
	HashFunc.h

check_PROGRAMS = ConfCreateTest ConfReadTest AllConfCreateTest AllConfReadTest AllConfUpdateTest

ConfCreateTest_SOURCES=ConfCreateTest.cpp
ConfReadTest_SOURCES=ConfReadTest.cpp
AllConfCreateTest_SOURCES=AllConfCreateTest.cpp
AllConfReadTest_SOURCES=AllConfReadTest.cpp
AllConfUpdateTest_SOURCES=AllConfUpdateTest.cpp

LDADD = libffdbfile.a ../filehash/libfilehash.a -lpthread

clean-local:
	-rm -rf *~ core*