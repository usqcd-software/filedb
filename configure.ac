# configure.ac file for FFDB package
AC_INIT([ffdbfile],[0.95],[ffdbfile authors <chen@jlab.org>],[ffdbfile])
AC_COPYRIGHT([Jefferson Science Assoicates, LLC. Jefferson Lab])
AC_MSG_NOTICE([Configuring ffdb-file])

AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE([-Wall -Werror])

## Get compiling host information
BUILD_OS=linux
AC_CANONICAL_BUILD
# Check whether this is a linux machine
case $build in
     *linux*)
     AC_MSG_NOTICE([This is a linux machine.])
     ;;
     *)
     AC_MSG_WARN([This package has not been tried on your platform $build !])
     BUILD_OS=$build_os
     ;;
esac

AC_PROG_CC

AC_PROG_CXX

AC_LANG(C,C++)

AM_PROG_AS

AC_PROG_INSTALL

AC_PROG_RANLIB

AC_CHECK_TOOL(AR, ar, [ar])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_C_VOLATILE

if test "X{$LDFLAGS}X" != "XX"
then
	SAVELDFLAGS=$LDFLAGS
fi

if test "X{$CPPFLAGS}X" != "XX"
then
	SAVECPPFLAGS=$CPPFLAGS
fi

# Check big/litte endian
FFDB_BYTE_ORDER=UNKNOWN
AC_C_BIGENDIAN([FFDB_BYTE_ORDER=BIG_ENDIAN],[FFDB_BYTE_ORDER=LITTLE_ENDIAN],[FFDB_BYTE_ORDER=UNKNOWN],[FFDB_BYTE_ORDER=UNIV])

if test "$FFDB_BYTE_ORDER" = BIG_ENDIAN
then
	AC_MSG_NOTICE([Compile FFDB for big endian machines.])
	AC_SUBST(FFDB_BYTE_ORDER_DEF,[-D_FFDB_BIG_ENDIAN])
elif test "$FFDB_BYTE_ORDER" = LITTLE_ENDIAN
then
	AC_MSG_NOTICE([Compile FFDB for little endian machines.])
	AC_SUBST(FFDB_BYTE_ORDER_DEF,[-D_FFDB_LITTLE_ENDIAN])
else
	AC_MSG_ERROR([Cannot determine endian type of this machine.])
fi

AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(filehash/Makefile)
AC_CONFIG_FILES(src/Makefile)

AC_OUTPUT
