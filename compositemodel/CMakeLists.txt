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

PROJECT(GoCompositeModel)


# Include directories

INCLUDE_DIRECTORIES(
  ${GoCompositeModel_SOURCE_DIR}/include
  ${parametrization_SOURCE_DIR}/include
  ${GoTopology_SOURCE_DIR}/include
  ${GoIntersections_SOURCE_DIR}/include
  ${GoImplicitization_SOURCE_DIR}/include
  ${GoIgeslib_SOURCE_DIR}/include
  ${GoToolsCore_SOURCE_DIR}/include
  ${GoTools_COMMON_INCLUDE_DIRS}
  )


# Linked in libraries

SET(DEPLIBS
  parametrization
  GoTopology
  GoIntersections
  GoImplicitization
  GoIgeslib
  GoToolsCore
  ttl
  sisl
  newmat
  )


# Make the GoCompositeModel library

FILE(GLOB_RECURSE GoCompositeModel_SRCS src/*.C include/*.h)
ADD_LIBRARY(GoCompositeModel ${GoCompositeModel_SRCS})
TARGET_LINK_LIBRARIES(GoCompositeModel ${DEPLIBS})
SET_PROPERTY(TARGET GoCompositeModel
  PROPERTY FOLDER "GoCompositeModel/Libs")
SET_TARGET_PROPERTIES(GoCompositeModel PROPERTIES SOVERSION ${GoTools_ABI_VERSION})



# Apps and tests
MACRO(ADD_APPS SUBDIR PROPERTY_FOLDER IS_TEST)
  FILE(GLOB_RECURSE GoCompositeModel_APPS ${SUBDIR}/*.C)
  FOREACH(app ${GoCompositeModel_APPS})
    GET_FILENAME_COMPONENT(appname ${app} NAME_WE)
    ADD_EXECUTABLE(${appname} ${app})
    TARGET_LINK_LIBRARIES(${appname} GoCompositeModel ${DEPLIBS})
    SET_TARGET_PROPERTIES(${appname}
      PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SUBDIR})
    SET_PROPERTY(TARGET ${appname}
      PROPERTY FOLDER "GoCompositeModel/${PROPERTY_FOLDER}")
    IF(${IS_TEST})
      ADD_TEST(${appname} ${SUBDIR}/${appname}
		--log_format=XML --log_level=all --log_sink=../Testing/${appname}.xml)
    ENDIF(${IS_TEST})
  ENDFOREACH(app)
ENDMACRO(ADD_APPS)

IF(GoTools_COMPILE_APPS)
  ADD_APPS(app "Apps" FALSE)
  ADD_APPS(examples "Examples" FALSE)
ENDIF(GoTools_COMPILE_APPS)

IF(GoTools_COMPILE_TESTS)
  SET(DEPLIBS ${DEPLIBS} ${Boost_LIBRARIES})
  ADD_APPS(unit_tests "Unit Tests" TRUE)
  ADD_APPS(regression_tests "Regression Tests" TRUE)
ENDIF(GoTools_COMPILE_TESTS)


# Copy data
FILE(COPY ${GoCompositeModel_SOURCE_DIR}/data
  DESTINATION ${GoCompositeModel_BINARY_DIR})
FILE(COPY ${GoCompositeModel_SOURCE_DIR}/examples/data
  DESTINATION ${GoCompositeModel_BINARY_DIR}/examples)


# 'install' target

IF(WIN32)
  # Windows
  # lib
  INSTALL(TARGETS GoCompositeModel DESTINATION ${GoTools_INSTALL_PREFIX}/lib)
  #include
  INSTALL(DIRECTORY include/GoTools/compositemodel
    DESTINATION ${GoTools_INSTALL_PREFIX}/include/GoTools
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
    )
ELSE(WIN32)
  # Linux
  #lib
  INSTALL(TARGETS GoCompositeModel DESTINATION lib COMPONENT compositemodel)
  # include
  INSTALL(DIRECTORY include/GoTools/compositemodel
    DESTINATION include/GoTools
    COMPONENT compositemodel-dev
    FILES_MATCHING PATTERN "*.h"
    PATTERN ".svn" EXCLUDE
    )
ENDIF(WIN32)

SET(CPACK_STRIP_FILES ${CPACK_STRIP_FILES} libGoCompositeModel.so)
