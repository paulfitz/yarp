# Copyright: (C) 2013 iCub Facility
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
add_definitions(-DYARP_tcpros_carrier_API=foo)
add_executable(yarpros2 yarpros.cpp
  RosLookup.cpp
  RosType.cpp
  RosTypeCodeGenYarp.cpp)

#INSTALL(TARGETS yarpros COMPONENT utilities DESTINATION bin)

option(TEST_yarpidl_rosmsg "Test rosmsg-based IDL" OFF)
if (TEST_yarpidl_rosmsg)
#  add_subdirectory(tests)
endif ()
