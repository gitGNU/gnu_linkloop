#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# Copyright (C) 2004, 2005, 2006, 2007 Oron Peled
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#

AC_PREREQ(2.59)
AC_INIT(linkloop, 0.0.3, oron@actcom.co.il)
AC_CONFIG_AUX_DIR([autotools_aux])
AC_CONFIG_SRCDIR([linkloop.c])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_CPP

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h inttypes.h netinet/in.h stdlib.h string.h sys/ioctl.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([alarm memset socket ether_hostton strtol])
AC_REPLACE_FUNCS([ether_hostton getopt])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
