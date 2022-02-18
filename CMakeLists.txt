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
PROJECT(SharemindModAlgorithms VERSION 1.4.0 LANGUAGES CXX)

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.6 REQUIRED)


FIND_PACKAGE(SharemindCHeaders 1.3.0 REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders 0.9.0 REQUIRED)
FIND_PACKAGE(SharemindLibSoftfloat 0.3.0 REQUIRED)
FIND_PACKAGE(SharemindLibSoftfloatMath 0.2.0 REQUIRED)
FIND_PACKAGE(SharemindLibSortNetwork 0.4.0 REQUIRED)
FIND_PACKAGE(SharemindModuleApis 1.1.0 REQUIRED)


# The module:
FILE(GLOB SharemindModAlgorithms_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB SharemindModAlgorithms_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
SharemindAddSharedLibrary(ModAlgorithms
    MODULE
    OUTPUT_NAME "sharemind_mod_algorithms"
    SOURCES
        ${SharemindModAlgorithms_SOURCES}
        ${SharemindModAlgorithms_HEADERS}
)
TARGET_INCLUDE_DIRECTORIES(ModAlgorithms
    INTERFACE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> # TODO
        $<INSTALL_INTERFACE:include>
    )
TARGET_LINK_LIBRARIES(ModAlgorithms
    PRIVATE
        Sharemind::CHeaders
        Sharemind::CxxHeaders
        Sharemind::LibSoftfloat
        Sharemind::LibSoftfloatMath
        Sharemind::LibSortNetwork
        Sharemind::ModuleApis
    )

# Configuration files:
INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packaging/configs/sharemind/"
        DESTINATION "/etc/sharemind/"
        COMPONENT "lib"
        EXCLUDE_FROM_ALL)


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("lib"
    NAME "libsharemind-mod-algorithms"
    DESCRIPTION "Sharemind Algorithms module"
    DEB_SECTION "libs"
    DEB_DEPENDS
        "libc6 (>= 2.19)"
        "libsharemind-softfloat (>= 0.3.0)"
        "libsharemind-softfloat-math (>= 0.2.0)"
        "libsharemind-sortnetwork (>= 0.3.0)"
        "libstdc++6 (>= 4.8.0)"
    DEB_EXTRA_CONTROL_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/conffiles"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-mod-algorithms-dbg"
    DESCRIPTION "Sharemind Algorithms module debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS
        "libsharemind-mod-algorithms (= ${SharemindModAlgorithms_DEB_lib_PACKAGE_VERSION})"
)
SharemindPackagingFinalize()
