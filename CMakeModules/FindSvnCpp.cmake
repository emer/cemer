# This file was obtained from the KDevelop project on 2012-10-29 from this URL:
# svn://svn.kde.org/home/kde/trunk/KDE/kdevplatform/cmake/modules -r 793973

# The following license was copied from the COPYING-CMAKE-SCRIPTS file
# at the same location:

#------------------------------------------------------------------------------
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#------------------------------------------------------------------------------

# Remainder of file is contents of FindSvnCpp.cmake
#------------------------------------------------------------------------------

# - Try to find svncpp libraries
#
#  This macro uses the following variables as preference for seraching the
#  headers and includes:
#  SVNCPP_PATH - root directory where svncpp is installed (ususally /usr)
#
#  The variables set by this macro are:
#  SVNCPP_FOUND              system has svncpp libraries
#  SVNCPP_INCLUDE_DIR        the include directory to link to svncpp
#  SVNCPP_LIBRARY            The library needed to link to svncpp
#  SVNCPP_VERSION            The version of svncpp that was found in hexadecimal form
#                            NOTE: This is just a guess for svncpp < 0.9.8
#                            based on the SOVERSION it will be set to 0x000904 or 0x000904

if(SVNCPP_INCLUDE_DIR AND SVNCPP_LIBRARY)
    # Already in cache, be silent
    set(SvnCpp_FIND_QUIETLY TRUE)
endif(SVNCPP_INCLUDE_DIR AND SVNCPP_LIBRARY)

if( SVNCPP_PATH )
    set(_SVNCPP_EXTRA_LIB_PATHS ${SVNCPP_PATH}/lib  )
    set(_SVNCPP_EXTRA_INC_PATHS ${SVNCPP_PATH}/include  )
endif( SVNCPP_PATH )

if(WIN32)
    set(_SVNCPP_EXTRA_LIB_PATHS ${_SVNCPP_EXTRA_LIB_PATHS} "$ENV{ProgramFiles}/svncpp/lib  )
    set(_SVNCPP_EXTRA_INC_PATHS ${_SVNCPP_EXTRA_LIB_PATHS} "$ENV{ProgramFiles}/svncpp/include  )
endif(WIN32)

FIND_PATH(SVNCPP_INCLUDE_DIR svncpp/client.hpp
    PATHS
        ${_SVNCPP_EXTRA_INC_PATHS}
    NO_DEFAULT_PATH
)
FIND_PATH(SVNCPP_INCLUDE_DIR svncpp/client.hpp)

FIND_LIBRARY(SVNCPP_LIBRARY svncpp
    PATHS
        ${_SVNCPP_EXTRA_LIB_PATHS}
    NO_DEFAULT_PATH
)
FIND_LIBRARY(SVNCPP_LIBRARY svncpp)




##############################
# Setup the result variables #
##############################

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args( SvnCpp DEFAULT_MSG SVNCPP_LIBRARY SVNCPP_INCLUDE_DIR )

if(EXISTS "${SVNCPP_LIBRARY}.1")
    set(SVNCPP_VERSION "0x000906")
else(EXISTS "${SVNCPP_LIBRARY}.1")
    set(SVNCPP_VERSION "0x000904")
endif(EXISTS "${SVNCPP_LIBRARY}.1")


set(SVNCPP_LIBRARY ${SVNCPP_LIBRARY} CACHE STRING "svncpp library")
set(SVNCPP_INCLUDE_DIR ${SVNCPP_INCLUDE_DIR} CACHE STRING "svncpp include directories")

mark_as_advanced(
  SVNCPP_LIBRARY
  SVNCPP_INCLUDE_DIR
)


#kate: space-indent on; indent-width 2; tab-width: 2; replace-tabs on; auto-insert-doxygen on
