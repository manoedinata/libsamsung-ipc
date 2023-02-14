;;; Copyright © 2020 Denis Carikli <GNUtoo@cyberdimension.org>
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

;;; The guix.scm files typically contain package definitions that
;;; are not meant to be used as regular packages but are meant for
;;; testing or developing on the given project.
;;;
;;; For instance here it is used to build libsamsung-ipc in various
;;; configurations. Instead distributions would typically be interested
;;; in only one of the configurations.
;;;
;;; It also copies the sources that are present in the directory that
;;; contains the guix.scm, and builds them. Distributions would instead
;;; be interested in retrieving the source from git or releases tarballs.
;;;
;;; Once you have Guix installed, to build libsamsung-ipc with the
;;; guix.scm file, you can use the 'guix build --file=scripts/guix.scm'
;;; command, however as explained above, keep in mind that it will copy
;;; all the files in the same directory than guix.scm, so you might want
;;; to make sure that the sources are clean (with 'make distclean') if
;;; you already built libsamsung-ipc in that directory.
;;;
;;; While this file could also serve as a basis to make a libsamsung-ipc
;;; package in Guix, it is probably a good idea to wait until the API
;;; and ABI changes are complete, ie when the ipc_client has been passed
;;; to all the exported functions, and make a new release when that is
;;; done.

(use-modules (ice-9 popen)
             (ice-9 rdelim)
             (ice-9 regex)
             (ice-9 textual-ports)
             (sxml ssax input-parse)
             ((guix licenses)
              #:prefix license:)
             (guix build-system android-ndk)
             (guix build-system gnu)
             (guix gexp)
             (guix git-download)
             (guix packages)
             (guix profiles)
             (guix transformations)
             (gnu packages)
             (gnu packages android)
             (gnu packages autotools)
             (gnu packages base)
             (gnu packages curl)
             (gnu packages disk)
             (gnu packages linux)
             (gnu packages llvm)
             (gnu packages pkg-config)
             (gnu packages python)
             (gnu packages python-xyz)
             (gnu packages tls)
             (gnu packages valgrind))

;; We need a more recent version of android-make-stub as it now
;; support passing LOCAL_MODULE= to make to build specific local
;; modules. This is needed because android-make-stub doesn't handle
;; dependencies, so we need to make sure that libsamsung-ipc is built
;; first. In addition we need a fix to make the applications that
;; depend on libsamsung-ipc find libsamsung-ipc's includes directory.
(define with-fixed-android-make-stub
  (options->transformation '((with-commit . "android-make-stub=4bc0e068c32fe525741da00af064ddc079de7e4b"))))

(define (parse-android.mk port modules-list)
  (let* ((line (read-line port)))
    (if (not (eof-object? line))
        (let* ((line-parts (string-split line #\space)))
          (if (string=? (car line-parts) "LOCAL_MODULE")
              (if (not (string=? (list-ref line-parts 2) "libsamsung-ipc"))
                  (if (string=? modules-list "")
                      (parse-android.mk port
                                        (list-ref line-parts 2))
                      (parse-android.mk port
                                        (string-append modules-list " "
                                                       (list-ref line-parts 2))))
                  (parse-android.mk port modules-list))
              (parse-android.mk port modules-list))) modules-list)))

(define android-local-modules-list
  (let* ((port (open-input-file "./Android.mk"))
         (local-modules (parse-android.mk port "")))
    (close-input-port port)
    (string-split local-modules #\space)))

(define %common-strict-cflags
  (let* ((port (open-input-pipe "./strict-cflags.sh"))
         (str (read-line port)))
    (close-pipe port)
    (string-append "CFLAGS=" str)))

(define %clang-strict-cflags
  (string-append "-Werror=non-virtual-dtor"))

(define %commit
  (let* ((port (open-input-pipe
                "git --no-pager log --oneline HEAD -1 --format='%H'"))
         (str (read-line port)))
    (close-pipe port) str))

(define %local-source
  (local-file (dirname (dirname (current-filename)))
              #:recursive? #t))

(define android-make
  #~(lambda (target android-local-modules)
      (lambda* (#:key inputs make-flags native-inputs outputs
                #:allow-other-keys)
        (substitute* "Android.mk"
          (("BUILD_SHARED_LIBRARY")
           "BUILD_HOST_SHARED_LIBRARY")
          (("BUILD_EXECUTABLE")
           "BUILD_HOST_EXECUTABLE"))
        ((assoc-ref %standard-phases target)
         #:inputs inputs
         #:outputs outputs
         #:make-flags (append make-flags
                              '("LDFLAGS=-lssl -lcrypto"
                                "LOCAL_MODULE=libsamsung-ipc"
                                "SO=libsamsung-ipc.so")))
        (map-in-order (lambda (local-module)
                        ((assoc-ref %standard-phases target)
                         #:inputs inputs
                         #:outputs outputs
                         #:make-flags (append make-flags
                                              (list (string-append
                                                     "LDFLAGS=-Wl,-rpath="
                                                     #$output
                                                     "/lib -lssl -lcrypto -L .")
                                                    (string-append
                                                     "LOCAL_MODULE="
                                                     local-module)))))
                      android-local-modules))))

(define android-phases
  #~(modify-phases %standard-phases
      (delete 'bootstrap)
      (replace 'build
        (#$android-make
         'build
         '#$android-local-modules-list))
      (replace 'install
        (#$android-make
         'install
         '#$android-local-modules-list))))

(define-public libsamsung-ipc
  (package
    (name "libsamsung-ipc")
    (version (git-version "0.0" "HEAD" %commit))
    (source %local-source)
    (build-system gnu-build-system)
    (native-inputs (list autoconf
                         automake
                         ddrescue
                         `(,(canonical-package glibc) "debug")
                         libtool
                         pkg-config
                         python
                         python-sh
                         valgrind))
    (inputs (list curl openssl))
    (arguments
     (list #:phases #~(modify-phases %standard-phases
                        (add-before 'build 'fix-valgrind
                          (lambda _
                            (substitute* (find-files "." ".*\\.py$")
                              (("'--leak-check=full',")
                               (string-append
                                "'--leak-check=full', '--extra-debuginfo-path="
                                (ungexp (this-package-native-input "glibc")
                                        "debug") "/lib/debug',"))))))
           #:configure-flags #~(list "--enable-debug"
                                     "--enable-valgrind-tests")))
    (synopsis
     "libsamsung-ipc is a free software implementation of the Samsung IPC modem protocol")
    (description
     "libsamsung-ipc is a free software implementation of the Samsung IPC modem protocol,
found in many Samsung smartphones and tablets.")
    (home-page "https://www.replicant.us")
    (license license:gpl2+)))

(define-public libsamsung-ipc-gcc-android
  (package
    (inherit libsamsung-ipc)
    (name "libsamsung-ipc-gcc-android")
    (build-system android-ndk-build-system)
    (inputs (list android-libutils curl openssl))
    (native-inputs '())
    (arguments
     (list #:make-flags #~(list #$%common-strict-cflags
                                #$%clang-strict-cflags)
           #:phases android-phases))))

(define-public libsamsung-ipc-gcc-autotools
  (package
    (inherit libsamsung-ipc)
    (name "libsamsung-ipc-gcc-autotools")
    (arguments
     (list #:phases #~(modify-phases %standard-phases
                        (add-before 'build 'patch-python
                          (lambda _
                            (substitute* (find-files "." ".*\\.py$")
                              (("/usr/bin/env python")
                               (which "python3"))) #t)))
           #:configure-flags #~(list "--enable-debug")
           #:make-flags #~(list #$%common-strict-cflags)))))

(define-public libsamsung-ipc-clang-autotools
  (package
    (inherit libsamsung-ipc)
    (name "libsamsung-ipc-gcc-autotools")
    (native-inputs (list autoconf
                         automake
                         ddrescue
                         libtool
                         pkg-config
                         python
                         python-sh
                         clang))
    (arguments
     (list #:phases #~(modify-phases %standard-phases
                        (add-before 'build 'patch-python
                          (lambda _
                            (substitute* (find-files "." ".*\\.py$")
                              (("/usr/bin/env python")
                               (which "python3"))) #t)))
           #:configure-flags #~(list "--enable-debug")
           #:make-flags #~(list #$%common-strict-cflags
                                #$%clang-strict-cflags)))))

(define-public libsamsung-ipc-clang-android
  (package
    (inherit libsamsung-ipc)
    (name "libsamsung-ipc-clang-android")
    (build-system android-ndk-build-system)
    (inputs (list android-libutils clang curl openssl))
    (native-inputs '())
    (arguments
     (list #:make-flags #~(list #$%common-strict-cflags
                                #$%clang-strict-cflags)
           #:phases android-phases))))

;; Combinaisons:
;; +--------------------------------+----------+----------+--------------+--------------+
;; | Package name                   | Compiler | Compiler | Build system | Comments     |
;; |                                |          | flags    |              |              |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc                 | GCC      | none     | autotools    | Base package |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc-gcc-android     | GCC      | strict   | Android.mk   |              |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc-gcc-autotools   | GCC      | strict   | autotools    |              |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc-clang-android   | clang    | strict   | Android.mk   |              |
;; +--------------------------------+----------+----------+--------------+--------------+
;; | libsamsung-ipc-clang-autotools | clang    | strict   | autotools    |              |
;; +--------------------------------+----------+----------+--------------+--------------+

(list libsamsung-ipc
      (with-fixed-android-make-stub libsamsung-ipc-clang-android)
      (with-fixed-android-make-stub libsamsung-ipc-gcc-android)
      libsamsung-ipc-clang-autotools libsamsung-ipc-gcc-autotools)
