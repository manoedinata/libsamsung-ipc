;;; Copyright © 2021 Denis Carikli <GNUtoo@cyberdimension.org>
;;;
;;; This file is free software; you can redistribute it and/or modify it
;;; under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 3 of the License, or (at
;;; your option) any later version.
;;;
;;; This file is distributed in the hope that it will be useful, but
;;; WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with GNU Guix.  If not, see <http://www.gnu.org/licenses/>.
;;; The guix.scm file is a convention: A file named guix.scm is
;;; found in several project source code, either in the top
;;; directory or in sub-directories like contrib for instance.

;;; This file can be used to setup a development environment for Guix
;;; users with the following command:
;;; guix shell --pure --container -f scripts/manifest.scm

(use-modules
 (gnu packages autotools)
 (gnu packages base)
 (gnu packages code)
 (gnu packages commencement)
 (gnu packages compression)
 (gnu packages curl)
 (gnu packages disk)
 (gnu packages gawk)
 (gnu packages pkg-config)
 (gnu packages python)
 (gnu packages python-xyz)
 (gnu packages tls))

(list
 autoconf
 automake
 bzip2
 coreutils
 curl
 ddrescue
 diffutils
 findutils
 gawk
 gzip
 gcc-toolchain
 grep
 lcov
 libtool
 gnu-make
 openssl
 pkg-config
 python
 python-sh
 sed
 tar
 xz)
