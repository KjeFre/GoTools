#
# Copyright (C) 1998, 2000-2007, 2010, 2011, 2012, 2013 SINTEF ICT,
# Applied Mathematics, Norway.
#
# Contact information: E-mail: tor.dokken@sintef.no                      
# SINTEF ICT, Department of Applied Mathematics,                         
# P.O. Box 124 Blindern,                                                 
# 0314 Oslo, Norway.                                                     
#
# This file is part of GoTools.
#
# GoTools is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version. 
#
# GoTools is distributed in the hope that it will be useful,        
# but WITHOUT ANY WARRANTY; without even the implied warranty of         
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public
# License along with GoTools. If not, see
# <http://www.gnu.org/licenses/>.
#
# In accordance with Section 7(b) of the GNU Affero General Public
# License, a covered work must retain the producer line in every data
# file that is created or manipulated using GoTools.
#
# Other Usage
# You can be released from the requirements of the license by purchasing
# a commercial license. Buying such a license is mandatory as soon as you
# develop commercial activities involving the GoTools library without
# disclosing the source code of your own applications.
#
# This file may be used in accordance with the terms contained in a
# written agreement between you and SINTEF ICT. 
#

PROJECT(GoIsogeometricModel)


# Include directories

INCLUDE_DIRECTORIES(
  ${GoIsogeometricModel_SOURCE_DIR}/include
  ${GoTrivariateModel_SOURCE_DIR}/include
  ${GoCompositeModel_SOURCE_DIR}/include
  ${parametrization_SOURCE_DIR}/include
  ${GoTopology_SOURCE_DIR}/include
  ${GoIntersections_SOURCE_DIR}/include
  ${GoImplicitization_SOURCE_DIR}/include
  ${GoIgeslib_SOURCE_DIR}/include
  ${GoTrivariate_SOURCE_DIR}/include
  ${GoToolsCore_SOURCE_DIR}/include
  ${GoTools_COMMON_INCLUDE_DIRS}
  )


# Linked in libraries

SET(DEPLIBS
  GoTrivariateModel
  GoCompositeModel
  parametrization
  GoTopology
  GoIntersections
  GoImplicitization
  GoIgeslib
  GoTrivariate
  GoToolsCore
  sisl
  newmat
  )


# Make the GoIsogeometricModel library

FILE(GLOB_RECURSE GoIsogeometricModel_SRCS src/*.C include/*.h)
ADD_LIBRARY(GoIsogeometricModel ${GoIsogeometricModel_SRCS})
TARGET_LINK_LIBRARIES(GoIsogeometricModel ${DEPLIBS})
SET_PROPERTY(TARGET GoIsogeometricModel
  PROPERTY FOLDER "GoIsogeometricModel/Libs")
SET_TARGET_PROPERTIES(GoIsogeometricModel PROPERTIES SOVERSION ${GoTools_ABI_VERSION})


# Apps, examples, tests, ...?
IF(GoTools_COMPILE_APPS)
  FILE(GLOB GoIsogeometricModel_APPS app/*.C)
  FOREACH(app ${GoIsogeometricModel_APPS})
    GET_FILENAME_COMPONENT(appname ${app} NAME_WE)
    ADD_EXECUTABLE(${appname} ${app})
    TARGET_LINK_LIBRARIES(${appname} GoIsogeometricModel ${DEPLIBS})
    SET_TARGET_PROPERTIES(${appname}
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY app)
    SET_PROPERTY(TARGET ${appname}
      PROPERTY FOLDER "GoIsogeometricModel/Apps")
  ENDFOREACH(app)

  FILE(GLOB GoIsogeometricModel_EXAMPLES examples/*.C)
  FOREACH(app ${GoIsogeometricModel_EXAMPLES})
    GET_FILENAME_COMPONENT(appname ${app} NAME_WE)
    ADD_EXECUTABLE(${appname} ${app})
    TARGET_LINK_LIBRARIES(${appname} GoIsogeometricModel ${DEPLIBS})
    SET_TARGET_PROPERTIES(${appname}
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY examples)
    SET_PROPERTY(TARGET ${appname}
      PROPERTY FOLDER "GoIsogeometricModel/Examples")
  ENDFOREACH(app)
ENDIF(GoTools_COMPILE_APPS)

# Copy data
ADD_CUSTOM_COMMAND(
  TARGET GoIsogeometricModel
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy_directory ${GoIsogeometricModel_SOURCE_DIR}/data
  ${GoIsogeometricModel_BINARY_DIR}/data
  )


# 'install' target

IF(WIN32)
  # Windows
  # lib
  INSTALL(TARGETS GoIsogeometricModel DESTINATION ${GoTools_INSTALL_PREFIX}/lib)
  # include
  INSTALL(DIRECTORY include/GoTools/isogeometric_model
    DESTINATION ${GoTools_INSTALL_PREFIX}/include/GoTools
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
    )
ELSE(WIN32)
  # Linux
  # lib
  INSTALL(TARGETS GoIsogeometricModel DESTINATION lib COMPONENT isogeometricmodel)
  # include
  INSTALL(DIRECTORY include/GoTools/isogeometric_model
    DESTINATION include/GoTools
    COMPONENT isogeometricmodel-dev
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
    )
ENDIF(WIN32)

SET(CPACK_STRIP_FILES ${CPACK_STRIP_FILES} libGoIsogeometricModel.so)
