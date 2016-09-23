#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([mylibs], [0.1], [stefan.fruhner@gmail.com])
AC_CONFIG_SRCDIR([Plane.hpp])
AC_CONFIG_HEADERS([config.h])



# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
#AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_LANG_PUSH([C++])

CXXFLAGS=""
# check if the compiler understands c++11 standard
m4_include([m4_ax_check_compile_flag.m4])
AX_CHECK_COMPILE_FLAG([-std=c++11], [
                        CXXFLAGS="$CXXFLAGS -std=c++11"]
                      )

# Checks for libraries.
## FIXME: Replace `main' with a function in `-lGL':
#AC_CHECK_LIB([GL], [main])
## FIXME: Replace `main' with a function in `-lGLU':
#AC_CHECK_LIB([GLU], [main])

# check for -lm, -lgslcblas and -lgsl
AC_CHECK_LIB(m,main)
AC_CHECK_LIB(gslcblas,main)
AC_CHECK_LIB(gsl,main,
[ # action-if-found
],
[ # action-if-not-found
AC_MSG_NOTICE([

*********************************************************************
* ATTENTION: Some features are disabled due to missing gsl library !*
*********************************************************************
]
)
echo "Continuing in 10 seconds ... "
sleep 10
]
)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h stdlib.h string.h sys/param.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([floor mkdir mkfifo pow sqrt strchr strerror strtol])

AC_CHECK_HEADER([initializer_list],
	[AC_DEFINE([HAVE_INITIALIZER_LIST], 1, [Define to 1 if you have <initializer_list>.])]
#	,[AC_MSG_ERROR([sorry, can't do anything for you])]
)

# check if colorgcc is installed
AC_CHECK_PROG([CGCC],[colorgcc], [colorgcc], g++)

AC_CHECK_PROG([GSLCFLAGS],[gsl-config], [\`gsl-config --cflags\` -DHAVE_LIBGSL],)
AC_CHECK_PROG([GSLLIBS],[gsl-config], [\`gsl-config --libs\`],)

#AC_CHECK_LIB([fltk], [main],[], [AC_MSG_ERROR([**** Could not find libfltk     ****])])
AC_CHECK_PROG([FLTKCONFIG],[fltk-config], [fltk-config], [AC_MSG_ERROR([**** I desperately need the fltk-config script.****])])

if test "$FLTKCONFIG" = "fltk-config"; then
  AC_CHECK_LIB([fltk], [main],[ ], [AC_MSG_ERROR([**** Could not find libfltk     ****])],[`fltk-config --ldflags`])
else
  AC_CHECK_LIB([fltk], [main],[], [AC_MSG_ERROR([**** Could not find libfltk     ****])])
fi

AC_CHECK_LIB([z], [main],[], [AC_MSG_ERROR([**** Could not find libz        ****])])
AC_CHECK_LIB([png], [main],[], [AC_MSG_ERROR([**** Could not find libpng        ****])])

AC_CHECK_PROG([STOW], [stow],[stow],[no])
if test $STOW = no; then
    AC_MSG_ERROR([ *** I like stow that much that I won't continue until you get used to it. *** ])
fi

AC_CHECK_PROG([DOXYGEN], [doxygen],[doxygen],[no])
if test $DOXYGEN = no; then
    AC_MSG_NOTICE([

    *** Documentation's included but you need *doxygen* ! ***

    ])
fi

AC_CONFIG_FILES([Makefile mylibs-config
                    myFl/Makefile myFl/myFl-config
                    myImg/Makefile myImg/myImg-config
                    Symbolic/Makefile Symbolic/Symbolic-config
                    Shape/Makefile Shape/Shape-config
                    MCubes/Makefile MCubes/MCubes-config])
AC_OUTPUT

AC_MSG_NOTICE([To install the mylibs library just

 $ make OPT=-O3 install

])
