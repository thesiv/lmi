#!/bin/sh

# Destroy any existing chroot named in './lmi_setup_inc.sh'.
#
# Copyright (C) 2016, 2017, 2018, 2019, 2020 Gregory W. Chicares.
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
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
#
# http://savannah.nongnu.org/projects/lmi
# email: <gchicares@sbcglobal.net>
# snail: Chicares, 186 Belle Woods Drive, Glastonbury CT 06033, USA

. ./lmi_setup_inc.sh
. /tmp/schroot_env

set -evx

assert_su
assert_not_chrooted

# umount expected mounts, then list any that seem to have been missed.
#
# It might seem snazzier to extract the relevant field of
#   grep "${CHRTNAME}" /proc/mounts
# and pipe it into
#   xargs umount
# but that would do something astonishing if two chroots (one nested
# and the other not) have mounted /proc thus:
#   proc /srv/chroot/"${CHRTNAME}"/proc
#   proc /srv/chroot/centos7lmi/srv/chroot/"${CHRTNAME}"/proc
# and only the non-nested one is intended to be destroyed.
#
# The 'findmnt' invocation is elaborated thus:
#   -r
# [see "column -t" below];
#   o SOURCE,TARGET
# because the natural order is {real-name, pseudonym}:
#   eric blair alias george orwell
# cf. /proc/self/mountstats:
#   device proc mounted on /srv/chroot/"${CHRTNAME}"/proc
# and thus, for commands, {existing, to-be-created}:
#   cp existing_file copied_file
#   ln -s original_name link_name
#   mount /dev/sda1 /
# but by default 'findmnt' reverses that order (its other default
# fields are generally less interesting for chroot bind mounts);
#   | grep "${CHRTNAME}"
# because 'findmnt -T' doesn't find substrings;
#   | sed -e's,^[/A-Za-z0-9_-]*[[]\([^]]*\)[]],\1,'
# because in output like
#   /dev/sdb5[/srv/cache_for_lmi]
#   /dev/mapper/VolGroup00-1v_root[/srv/cache_for_lmi]
# the part in brackets is the more interesting, while the "/dev..."
# part is distracting (and the '--nofsroot' option would discard the
# more interesting part)--yet bear in mind that the part in brackets
# may be incomplete if a leading component like '/srv' is mounted on
# a distinct device (and '--canonicalize' wouldn't fix that); and
#   | column -t
# along with '-r' because '-l' does a poor job of columnization.

umount /srv/chroot/"${CHRTNAME}"/srv/cache_for_lmi      || true
umount /srv/chroot/"${CHRTNAME}"/var/cache/apt/archives || true
umount /srv/chroot/"${CHRTNAME}"/dev/pts                || true
umount /srv/chroot/"${CHRTNAME}"/proc                   || true

findmnt -ro SOURCE,TARGET \
  | grep "${CHRTNAME}" \
  | sed -e's,^[/A-Za-z0-9_-]*[[]\([^]]*\)[]],\1,' \
  | column -t

# Abort if any $CHRTNAME mountpoint remains.
#
# This shouldn't occur, and 'rm --one-file-system' below should be
# safe anyway, yet an actual catastrophe did occur nonetheless.
findmnt | grep "${CHRTNAME}" && exit 9

# Use '--one-file-system' because it was designed for this use case:
#   https://lists.gnu.org/archive/html/bug-coreutils/2006-10/msg00332.html
# | This option is useful when removing a build "chroot" hierarchy
#
# These scripts create a chroot in /srv/chroot/"$CHRTNAME", but
# chroots need not be located in /srv , so use 'schroot --location'
# to attempt to detect any manual override. Such detection may fail
# (e.g., if the chroot's '.conf' file is missing).

loc0=/srv/chroot/"${CHRTNAME}"
loc1="$(schroot --chroot="${CHRTNAME}" --location)"
if [ -n "${loc1}" ] && [ "${loc0}" != "${loc1}" ]; then
  echo "chroot found in unexpected location--remove it manually"
  exit 9
fi

# Remove the directory that these scripts would create. Removing
# "${loc1}" wouldn't be appropriate because it can be an empty string.

rm --one-file-system --recursive --force "${loc0}"

# Explicitly test postcondition.
if [ -e "${loc0}" ] || [ -e "${loc1}" ] ; then echo "Oops."; exit 9; fi

# schroot allows configuration files in /etc/schroot/chroot.d/ only.

rm --force /etc/schroot/chroot.d/"${CHRTNAME}".conf

stamp=$(date -u +'%Y%m%dT%H%M%SZ')
echo "$stamp $0: Removed old chroot."  | tee /dev/tty
