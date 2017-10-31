# Prompt
# Copyright (C) 2017  Dridi Boukelmoune
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([prompt], [0.1])
AC_COPYRIGHT([Copyright (c) 2017 Dridi Boukelmoune])

AC_PREREQ([2.63])
AC_LANG_C
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([prompt.spec.in])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CHECK_LIB([readline],[readline],[],[AC_MSG_ERROR([missing libreadline])])
AC_CHECK_HEADERS([readline/history.h])
AC_CONFIG_FILES([Makefile prompt.spec])

AM_INIT_AUTOMAKE([1.11 foreign subdir-objects -Wall -Werror])
AM_PROG_AR
AM_SANITY_CHECK

AC_OUTPUT
