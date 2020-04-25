#!/bin/sh

# Attempt to set up a git repository to be shared by multiple users.
#
# FETCH_HEAD doesn't get the right permissions, and that's a git
# defect--see:
#   https://lists.nongnu.org/archive/html/lmi/2020-03/msg00016.html
#   https://public-inbox.org/git/20200319010321.18614-1-vz-git@zeitlins.org/T/#u

set -v

# Like 'share_git2.sh', but creates a bare repository.

# Expect 022 here:
umask
# The FETCH_HEAD permission problem doesn't arise if umask is 002,
# so one option is to execute
#   umask 002
# That affects only the shell in which this script runs, but it has a
# persistent effect if run at the command line.

# Start with a fresh throwaway directory.
cd /tmp || exit
rm -rf /tmp/eraseme
mkdir -p /tmp/eraseme
cd /tmp/eraseme || exit

# Expect group users to include 'pulse' as well as normal user:
getent group audio

# Get this over with early. Reason: if script is piped into 'less',
# type the password before the screen fills and password characters
# are treated as 'less' commands.
sudo --user=pulse true

# First method: emulate git-clone as three git commands, with
# a single 'chgrp' call at exactly the right spot.

inited="inited_bare"

# The crux of this method is 'git init':
git init --bare --shared "$inited"
chgrp -R audio "$inited"
git -C "$inited" remote add origin https://github.com/wxWidgets/zlib.git
git -C "$inited" fetch origin

find ./"$inited" ! -perm -g=w |sed -e'/objects\/pack/d'
# Oops: FETCH_HEAD doesn't have group write permissions:
ls -l ./"$inited"/*HEAD

# This isn't really necessary; it just makes the result look more like
# that of the second method, below.
git -C "$inited" pack-refs --all

# This succeeds when run by owner:
git -C "$inited" fetch
# this fails:
sudo --user=pulse git -C "$inited" fetch
# but it succeeds if FETCH_HEAD's permissions are fixed:
chmod g+w "$inited"/FETCH_HEAD
sudo --user=pulse git -C "$inited" fetch

# Second method: git-clone, then fix permissions manually--necessary
# despite '--config core.SharedRepository=group'.

chgrp audio .
chmod g+ws .

cloned="cloned_bare"

# The crux of this method is 'git clone':
git clone --jobs=32 --bare --config core.SharedRepository=group https://github.com/wxWidgets/zlib.git "$cloned"

# 'git clone' created its files with the intended group (so this:
#   chgrp -R audio "$cloned"
# isn't needed), but it didn't make them group writable.
# This is better than 'chmod -R g+s' (it affects only directories):
find "$cloned" -type d -exec chmod g+s {} +
# Specifying 's' here would cause many 'S' occurrences in 'ls' output:
#   chmod -R g+swX "$cloned"
# Specifying 'g+w' here would cause pack files to be group writable:
chmod -R g=u "$cloned"
# There, 'g=u' doesn't override the earlier 'g+s'--see:
#   https://lists.nongnu.org/archive/html/lmi/2020-03/msg00019.html

# Permissions seem to be okay...
find ./"$cloned" ! -perm -g=w |sed -e'/objects\/pack/d'
# ...but that's because FETCH_HEAD doesn't yet exist:
ls -l ./"$cloned"/*HEAD

# This succeeds when run by owner:
git -C "$cloned" fetch

find ./"$cloned" ! -perm -g=w |sed -e'/objects\/pack/d'
# Oops: FETCH_HEAD doesn't have group write permissions:
ls -l ./"$cloned"/*HEAD

# This fails:
sudo --user=pulse git -C "$cloned" fetch
# but it succeeds if FETCH_HEAD's permissions are fixed:
chmod g+w "$cloned"/FETCH_HEAD
sudo --user=pulse git -C "$cloned" fetch

# The two methods produce somewhat similar results. Sizes:
du -sb "$cloned" "$inited"
# are almost the same. Small differences:
#  - inited_bare/config has this extra line under [remote "origin"]:
#      fetch = +refs/heads/*:refs/remotes/origin/*
#    (which is just a default)
#  - HEAD is
#      refs/heads/master  [in inited_bare]
#      refs/heads/wx      [in cloned_bare]
#    though both seem to point to the same SHA1
#  - git-fsck complains about an unborn branch, in inited_bare only:
git -C "$cloned" fsck
git -C "$inited" fsck

# Show any files that aren't group writable, expecting '.' only.
#
# Something like this:
#   stat --printf="%A %a\t%U %G %n\n" $(find .) |sed ...
# could be used instead of 'ls', but the gain in robustness doesn't
# seem worth the loss in readability.
# shellcheck disable=SC2012
# shellcheck disable=SC2046
ls -ld $(find .) |sed -e'/^.....w/d' -e'/objects\/pack/d'

# List all files' permissions for comparison, e.g.:
#   meld /srv/chroot/bullseye0/tmp/eraseme/ls-* &
cd /tmp/eraseme/"$inited" && ls -alR >/tmp/eraseme/ls-"$inited"
cd /tmp/eraseme/"$cloned" && ls -alR >/tmp/eraseme/ls-"$cloned"
