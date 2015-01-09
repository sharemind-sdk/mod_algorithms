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

################################################################################
#
# CMake script for finding libsortnetwork.
# The default CMake search process is used to locate files.
#
# This script creates the following variables:
#  LIBSORTNETWORK_FOUND: Boolean that indicates if the package was found
#  LIBSORTNETWORK_INCLUDE_DIRS: Paths to the necessary header files
#  LIBSORTNETWORK_LIBRARIES: Package libraries
#
################################################################################

# Find headers and libraries
FIND_PATH(
    LIBSORTNETWORK_INCLUDE_DIR
    NAMES sn_network.h
    HINTS
        $ENV{LIBSORTNETWORK_ROOT}
        ${LIBSORTNETWORK_ROOT}
    PATHS
        /opt/local
        /usr/local
        /usr
    PATH_SUFFIXES
        include
)

FIND_LIBRARY(
    LIBSORTNETWORK_LIBRARY
    NAMES
        sortnetwork
    HINTS
        $ENV{LIBSORTNETWORK_ROOT}
        ${LIBSORTNETWORK_ROOT}
    PATHS
        /opt/local
        /usr/local
        /usr
    PATH_SUFFIXES
        lib
)

# libsortnetwork does not have version numbers exported
SET(LIBSORTNETWORK_VERSION_OK 1)

# Set LIBSORTNETWORK_FOUND honoring the QUIET and REQUIRED arguments
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    libsortnetwork
    "Could NOT find LIBSORTNETWORK"
    LIBSORTNETWORK_LIBRARY LIBSORTNETWORK_INCLUDE_DIR LIBSORTNETWORK_VERSION_OK)

# Output variables
IF(LIBSORTNETWORK_FOUND)
    # Include dirs
    SET(LIBSORTNETWORK_INCLUDE_DIRS ${LIBSORTNETWORK_INCLUDE_DIR})

    # Libraries
    SET(LIBSORTNETWORK_LIBRARIES ${LIBSORTNETWORK_LIBRARY})
ENDIF(LIBSORTNETWORK_FOUND)

# Advanced options for not cluttering the cmake UIs:
MARK_AS_ADVANCED(LIBSORTNETWORK_INCLUDE_DIR LIBSORTNETWORK_LIBRARY LIBSORTNETWORK_VERSION_OK)
