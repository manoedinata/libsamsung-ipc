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

die()
{
    echo "$@"
    exit 1;
}

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
}

build_and_git_rebase_continue()
{
    commits="$1"
    commit=1

    while true ; do
        print_header "[ ${commit} of ${commits} ]"
        guix build --file=scripts/guix.scm || die "guix build failed"
        commit="$(expr ${commit} + 1)"
        git rebase --continue || break
    done
}

if [ $# -eq 2 ] && [ "$1" == "build-from" ] ; then
    base_commit="$2"
    nr_commits="$(git log --oneline ${base_commit}~1..HEAD --oneline | wc -l)"
    GIT_EDITOR="sed 's#^pick #edit #g' -i " git rebase -i "${base_commit}"
    build_and_git_rebase_continue "${nr_commits}"
elif [ $# -eq 1 ] && [ "$1" == "continue" ] ; then
    nr_commits="$(git log --oneline REBASE_HEAD~1..ORIG_HEAD --oneline | wc -l)"
    build_and_git_rebase_continue "${nr_commits}"
else
    usage
fi
