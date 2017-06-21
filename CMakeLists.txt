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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(SHAREMIND_MOD_ALGORITHMS CXX)

SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(SHAREMIND_MOD_ALGORITHMS_SOVERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
SET(SHAREMIND_MOD_ALGORITHMS_VERSION
    "${SHAREMIND_MOD_ALGORITHMS_SOVERSION}.${CPACK_PACKAGE_VERSION_PATCH}")

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeIncludes/"
                      ${CMAKE_MODULE_PATH})

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

ADD_COMPILE_OPTIONS(
  "-std=c++11" "-Wall" "-Wextra" "-O2" "-fPIC"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-ggdb>"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-fomit-frame-pointer>"
)

FIND_PACKAGE(libsortnetwork REQUIRED)
FIND_PACKAGE(SharemindCHeaders REQUIRED)
FIND_PACKAGE(SharemindCxxHeaders REQUIRED)
FIND_PACKAGE(SharemindLibmodapi REQUIRED)
FIND_PACKAGE(SharemindLibsoftfloat REQUIRED)
FIND_PACKAGE(SharemindLibsoftfloatMath REQUIRED)

INCLUDE_DIRECTORIES(
    ${LIBSORTNETWORK_INCLUDE_DIRS}
    ${SharemindCHeaders_INCLUDE_DIRS}
    ${SharemindCxxHeaders_INCLUDE_DIRS}
    ${SharemindLibmodapi_INCLUDE_DIRS}
    ${SharemindLibsoftfloat_INCLUDE_DIRS}
    ${SharemindLibsoftfloatMath_INCLUDE_DIRS}
)

ADD_DEFINITIONS(
    ${SharemindCHeaders_DEFINITIONS}
    ${SharemindCxxHeaders_DEFINITIONS}
    ${SharemindLibmodapi_DEFINITIONS}
    ${SharemindLibsoftfloat_DEFINITIONS}
    ${SharemindLibsoftfloatMath_DEFINITIONS}
)

FILE(GLOB SHAREMIND_MOD_ALGORITHMS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB SHAREMIND_MOD_ALGORITHMS_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

ADD_LIBRARY(sharemind_mod_algorithms MODULE ${SHAREMIND_MOD_ALGORITHMS_SOURCES}
                                            ${SHAREMIND_MOD_ALGORITHMS_HEADERS})
TARGET_LINK_LIBRARIES(sharemind_mod_algorithms
    ${LIBSORTNETWORK_LIBRARIES}
    ${SharemindCHeaders_LIBRARIES}
    ${SharemindCxxHeaders_LIBRARIES}
    ${SharemindLibsoftfloat_LIBRARIES}
    ${SharemindLibsoftfloatMath_LIBRARIES}
)
SET_TARGET_PROPERTIES("sharemind_mod_algorithms" PROPERTIES
                      OUTPUT_NAME "sharemind_mod_algorithms"
                      VERSION "${SHAREMIND_MOD_ALGORITHMS_VERSION}"
                      SOVERSION "${SHAREMIND_MOD_ALGORITHMS_SOVERSION}")
INSTALL(TARGETS sharemind_mod_algorithms LIBRARY DESTINATION "lib")


# Packaging:
SET(CPACK_PACKAGE_VENDOR "Cybernetica AS")
SET(CPACK_PACKAGE_CONTACT "Sharemind packaging <sharemind-packaging@cyber.ee>")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Sharemind Algorithms module")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_NAME "libsharemind-mod-algorithms")
SET(CPACK_DEBIAN_PACKAGE_RELEASE "cyber1")
SET(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libc6 (>= 2.19), libsortnetwork (>= 1.1.0), libstdc++6 (>= 4.8.0)")
SET(CPACK_DEBIAN_PACKAGE_SECTION "libs")
SET(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
INCLUDE(CPack)
