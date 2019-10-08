#!/bin/sh

# Create a chroot for redhat-7.
#
# Copyright (C) 2019 Gregory W. Chicares.
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

set -evx

stamp0=$(date -u +'%Y-%m-%dT%H:%M:%SZ')
echo "Started: $stamp0"

# First, destroy any chroot left by a prior run.
grep "${CHRTNAME}" /proc/mounts | cut -f2 -d" " | xargs umount || echo "None?"
rm -rf /srv/chroot/"${CHRTNAME}"
rm /etc/schroot/chroot.d/"${CHRTNAME}".conf || echo "None?"
umount /srv

mount -t tmpfs -o size=10G tmpfs /srv
findmnt /tmp

# Make a more modern 'git' available via 'scl'. This is not needed
# if all real work is done in a debian chroot.
#yum --assumeyes install centos-release-scl
#yum-config-manager --enable rhel-server-rhscl-7-rpms
#yum --assumeyes install devtoolset-8 rh-git218
# In order to use the tools on the three preceding lines, do:
#   scl enable devtoolset-8 rh-git218 $SHELL
# and then they'll be available in that environment.

# Fix weird errors like "Problem with the SSL CA cert (path? access rights?)".
yum --assumeyes install ca-certificates curl nss-pem

# Install "EPEL" by using 'rpm' directly [historical]. See:
#   https://lists.nongnu.org/archive/html/lmi/2019-09/msg00037.html
#rpm -ivh https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
# Instead, use 'yum' to install "EPEL".
#yum --assumeyes install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
yum --assumeyes install epel-release

yum --assumeyes install schroot
# To show available debootstrap scripts:
#   ls /usr/share/debootstrap/scripts

# Install a debian chroot inside this centos chroot.
yum --assumeyes install debootstrap
mkdir -p /srv/chroot/"${CHRTNAME}"
debootstrap "${CODENAME}" /srv/chroot/"${CHRTNAME}" http://deb.debian.org/debian/

echo Installed debian "${CODENAME}".

cat >/etc/schroot/chroot.d/"${CHRTNAME}".conf <<EOF
[${CHRTNAME}]
aliases=lmi
description=debian ${CODENAME} cross build ${CHRTVER}
directory=/srv/chroot/${CHRTNAME}
users="${NORMAL_USER}"
groups="${NORMAL_USER}"
root-groups=root
type=plain
EOF

mkdir -p /srv/chroot/"${CHRTNAME}"/cache_for_lmi
mount --bind /srv/cache_for_lmi /srv/chroot/"${CHRTNAME}"/cache_for_lmi || echo "Oops."

mkdir -p /var/cache/"${CODENAME}"
mount --bind /var/cache/"${CODENAME}" /srv/chroot/"${CHRTNAME}"/var/cache/apt/archives || echo "Oops."

# For a server that, bizarrely, blocks gnu.org but allows github.com:
# %s,https://git.savannah.nongnu.org/cgit/lmi.git/plain/,https://github.com/vadz/lmi/raw/master/,
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_10.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_11.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_20.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_21.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_30.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_40.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_41.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_42.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_43.sh'
wget -N 'https://github.com/vadz/lmi/raw/master/lmi_setup_inc.sh'
chmod +x lmi_setup_*.sh

for z in lmi_setup_*.sh; do \
  sed -i "$z" -e 's,https://git.savannah.nongnu.org/cgit/lmi.git/plain/,https://github.com/vadz/lmi/raw/master/,'
done

. ./lmi_setup_inc.sh

set -evx

# ./lmi_setup_10.sh
# ./lmi_setup_11.sh
cp -a lmi_setup_*.sh /srv/chroot/${CHRTNAME}/tmp
schroot --chroot=${CHRTNAME} --user=root --directory=/tmp ./lmi_setup_20.sh
schroot --chroot=${CHRTNAME} --user=root --directory=/tmp ./lmi_setup_21.sh
# sudo -u "${NORMAL_USER}" ./lmi_setup_30.sh
schroot --chroot=${CHRTNAME} --user="${NORMAL_USER}" --directory=/tmp ./lmi_setup_40.sh
schroot --chroot=${CHRTNAME} --user="${NORMAL_USER}" --directory=/tmp ./lmi_setup_41.sh
schroot --chroot=${CHRTNAME} --user="${NORMAL_USER}" --directory=/tmp ./lmi_setup_42.sh
schroot --chroot=${CHRTNAME} --user="${NORMAL_USER}" --directory=/tmp ./lmi_setup_43.sh

stamp1=$(date -u +'%Y-%m-%dT%H:%M:%SZ')
echo "Finished: $stamp1"

seconds=$(($(date '+%s' -d "$stamp1") - $(date '+%s' -d "$stamp0")))
elapsed=$(date -u -d @"$seconds" +'%H:%M:%S')
echo "Elapsed: $elapsed"
