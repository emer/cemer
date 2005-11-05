#                                         PDP_DETERMINE_OSTYPE
#*************************************************************
# Compiler and linker flags, includes and defines are set
# based on the results of these tests.
#*************************************************************
#
AC_DEFUN([PDP_DETERMINE_OSTYPE],[
AC_REQUIRE([AC_CANONICAL_BUILD])
AC_MSG_CHECKING([results of config.guess])
case "${build_os}" in
	*linux*)
		AC_SUBST([PDP_PLATFORM],[LINUX])
		AC_DEFINE([LINUX],[1],[When on linux])
	;;
	*darwin*)
		AC_SUBST([PDP_PLATFORM],[DARWIN])
		AC_DEFINE([DARWIN],[1],[When on darwin])
		AC_DEFINE([LINUX],[1],[When on darwin])
	;;
	*cygwin*)
		AC_SUBST([PDP_PLATFORM],[CYGWIN])
		AC_DEFINE([CYGWIN],[1],[When on cygwin])
		AC_DEFINE([LINUX],[1],[When on cygwin])
	;;
	*)
		AC_MSG_ERROR([PDP++ supports Linux, Darwin, and Windows],[1])
	;;
esac
AC_MSG_RESULT([$PDP_PLATFORM])
AM_CONDITIONAL([LINUX],[test $PDP_PLATFORM = LINUX])
AM_CONDITIONAL([DARWIN],[test $PDP_PLATFORM = DARWIN])
AM_CONDITIONAL([CYGWIN],[test $PDP_PLATFORM = CYGWIN])
]) #PDP_DETERMINE_OSTYPE
