#
# Copyright (C) 2015 Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(SharemindModAlgorithms VERSION 1.0.0 LANGUAGES CXX)

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers REQUIRED)

SharemindSetProjectVersion()


FIND_PACKAGE(SharemindCHeaders REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders REQUIRED)
FIND_PACKAGE(SharemindLibSoftfloat REQUIRED)
FIND_PACKAGE(SharemindLibSoftfloatMath REQUIRED)
FIND_PACKAGE(SharemindLibSortNetwork REQUIRED)
FIND_PACKAGE(SharemindModuleApis REQUIRED)


# The module:
SharemindSetCxx11CompileOptions()
FILE(GLOB SharemindModAlgorithms_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB SharemindModAlgorithms_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
SharemindAddSharedLibrary("sharemind_mod_algorithms"
    MODULE
    OUTPUT_NAME "sharemind_mod_algorithms"
    SOURCES
        ${SharemindModAlgorithms_SOURCES}
        ${SharemindModAlgorithms_HEADERS}
    INCLUDE_DIRECTORIES
        ${SharemindCHeaders_INCLUDE_DIRS}
        ${SharemindCxxHeaders_INCLUDE_DIRS}
        ${SharemindLibSoftfloat_INCLUDE_DIRS}
        ${SharemindLibSoftfloatMath_INCLUDE_DIRS}
        ${SharemindLibSortNetwork_INCLUDE_DIRS}
        ${SharemindModuleApis_INCLUDE_DIRS}
    LEGACY_DEFINITIONS
        ${SharemindCHeaders_DEFINITIONS}
        ${SharemindCxxHeaders_DEFINITIONS}
        ${SharemindLibSoftfloat_DEFINITIONS}
        ${SharemindLibSoftfloatMath_DEFINITIONS}
        ${SharemindLibSortNetwork_DEFINITIONS}
    LINK_LIBRARIES
        ${LIBSORTNETWORK_LIBRARIES}
        ${SharemindCHeaders_LIBRARIES}
        ${SharemindCxxHeaders_LIBRARIES}
        ${SharemindLibSoftfloat_LIBRARIES}
        ${SharemindLibSoftfloatMath_LIBRARIES}
        ${SharemindLibSortNetwork_LIBRARIES}
)


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("lib"
    NAME "libsharemind-mod-algorithms"
    DESCRIPTION "Sharemind Algorithms module"
    DEB_SECTION "libs"
    DEB_DEPENDS
        "libc6 (>= 2.19)"
        "libsharemind-sortnetwork (>= 1.1.0)"
        "libstdc++6 (>= 4.8.0)"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-mod-algorithms-dbg"
    DESCRIPTION "Sharemind Algorithms module debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS
        "libsharemind-mod-algorithms (= ${SharemindModAlgorithms_VERSION})"
)
SharemindPackagingFinalize()
