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
bin_PROGRAMS		= linkloop linkloop_reply lanscan
linkloop_SOURCES	= common.c linkloop.c linkloop.h
linkloop_reply_SOURCES	= common.c linkloop_reply.c linkloop.h
lanscan_SOURCES		= common.c lanscan.c linkloop.h
man_MANS		= linkloop.1
EXTRA_DIST		= $(man_MANS)

#$(srcdir)/ChangeLog: $(SOURCES)
#	cd $(srcdir) && cvs2cl -w -t --no-wrap -S -l '-l'
