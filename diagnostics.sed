# Parse compiler output for diagnostics.
#
# Copyright (C) 2003, 2004, 2005 Gregory W. Chicares.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# http://savannah.nongnu.org/projects/lmi
# email: <chicares@cox.net>
# snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA
#
# $Id: diagnostics.sed,v 1.3 2005-08-23 00:12:46 chicares Exp $
#
  # Delete expected output from make.
/Circular.*dependency dropped/d
/Entering directory/d
/Leaving directory/d
/is up to date/d
/Nothing to be done for/d
/^GNUmakefile: MAKING/d
/^Makefile: MAKING/d
/submake[0-1]: MAKING/d
/submake[0-1]: with dependencies/d
/dependencies: /d
/^cp lmi/d
/^cp --preserve lmi/d
/^.:.gnu.cp --preserve lmi/d
/^mkdir /d
/^.:.gnu.mkdir /d
/^Begun at /d
/^Ended at /d
/^Elapsed time.*milliseconds/d
/^rm eraseme.o$/d
/^.:.gnu.rm eraseme.o$/d
/not remade because of errors\./d
/^These files are more recent than/d
/Built [0-9].*T[0-9].*Z\.$/d
/^make.*\[[0-9]*\]: \*\*\* \[.*\] Error [0-9]*$/d
/^make.*\[[0-9]*\]: \[.*\] Error [0-9]* (ignored)$/d
  # Delete expected output from gnu cpp.
/^[^ ]*CPP -x/d
/^[^ ]*cpp -x/d
  # TODO ?? Remove this workaround when we use a better shell
/.*cpp.*-x/d
  # Delete expected output from gcc build.
/^[^ ]*g\+\+.*-[Ico] /d
/^[^ ]*sed -e.* /d
/^windres --include-dir/d
/total time in link/d
/ld: data size /d
/^[^ ]*ar -[rus]* /d
/^[^ ]*ar: creating /d
/^Creating library file:.*.a$/d
/^< [a-z0-9].d0/d

