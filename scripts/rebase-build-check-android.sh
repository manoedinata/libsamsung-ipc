#!/bin/bash
# -*- coding: utf-8 -*-
# Copyright (C) 2022 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
set -e

usage()
{
    printf "Usage:\n"
    printf "%s build-from <base-commit-ref> " "$0"
    printf "# Build all commits since <base-commit-ref>\n" "%0"
    printf "%s continue                     " "$0"
    printf "# Continue rebasing and building commits\n" "%0"
    exit 64; # EX_USAGE from sysexits.h
}

print_header()
{
    set +e
    message="$1"

    len=$(expr $(echo -n ${message} | wc -c) + 2)

    nr_chars=$(expr $(expr 80 - ${len}) / 2)

    i="${nr_chars}"

    while [ ${i} -gt 0 ] ; do
	echo -n "#"
	i="$(expr ${i} - 1)"
    done

    echo -n " ${message} "

    i="${nr_chars}"

    while [ ${i} -gt 0 ] ; do
	echo -n "#"
	i="$(expr ${i} - 1)"
    done
    echo
    set -e
}

build_and_git_rebase_continue()
{
    total_commits="$1"

    total_targets="$(./vendor/replicant/build.sh targets | wc -l)"
    parallel_tasks=$(echo "$(grep 'processor' /proc/cpuinfo | wc -l ) + 1" | bc)

    local_modules="$(grep \
                        "^LOCAL_MODULE := " \
                        hardware/replicant/libsamsung-ipc/Android.mk | \
                        awk '{print $3}' | sort -u)"

    source build/envsetup.sh

    # Currently all the Replicant versions supported by libsamsung-ipc
    # have an aosp_arm-eng target. Using aosp_arm-eng also ensures
    # that TARGET_DEVICE isn't set by the device repository. This also
    # leaves us more freedom in libsamsung-ipc as we could for
    # instance decide to not build unused code based on TARGET_DEVICE.
    lunch aosp_arm-eng

    commit_nr=1
    while true ; do
        print_header "[ commit ${commit_nr} of ${total_commits} ]"
        make "-j${parallel_tasks}" ${local_modules}
        git -C hardware/replicant/libsamsung-ipc rebase --continue || break
	commit_nr="$(expr ${commit_nr} + 1)"
    done
}

if [ $# -eq 2 ] && [ "$1" == "build-from" ] ; then
    base_commit="$2"
    nr_commits="$(git -C hardware/replicant/libsamsung-ipc log --oneline \
		  ${base_commit}~1..HEAD --oneline | wc -l)"
    GIT_EDITOR="sed 's#^pick #edit #g' -i " \
              git -C hardware/replicant/libsamsung-ipc \
	      rebase -i "${base_commit}"

    build_and_git_rebase_continue "${nr_commits}"
elif [ $# -eq 1 ] && [ "$1" == "continue" ] ; then
    nr_commits="$(git -C hardware/replicant/libsamsung-ipc \
		  log --oneline REBASE_HEAD~1..ORIG_HEAD --oneline | wc -l)"
    build_and_git_rebase_continue "${nr_commits}"
else
    usage
fi
