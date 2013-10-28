# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (YARP_USE_PERSISTENT_NAMESERVER)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${SQLite_INCLUDE_DIRS})
  include_directories(${ACE_INCLUDE_DIRS})

  set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_name PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

  add_library(YARP_serversql STATIC 
    src/TripleSourceCreator.cpp 
    src/TripleSourceCreator.h 
    src/Triple.h 
    src/TripleSource.h 
    src/SqliteTripleSource.h 
    src/NameServiceOnTriples.cpp 
    src/NameServiceOnTriples.h 
    src/Allocator.h 
    src/AllocatorOnTriples.cpp 
    src/AllocatorOnTriples.h
    src/Subscriber.h
    src/SubscriberOnSql.h 
    src/SubscriberOnSql.cpp 
    src/ComposedNameService.h 
    src/ConnectThread.h
    src/ParseName.h
    src/ParseName.cpp
    src/StyleNameService.h 
    src/StyleNameService.cpp
    src/RosAdaptor.h
    src/RosAdaptor.cpp
    src/yarpserver3.cpp)
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_OS)
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_init)
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_name)
  target_link_libraries(YARP_serversql LINK_PRIVATE ${SQLite_LIBRARIES})
  if (UNIX)
    set_target_properties(YARP_serversql PROPERTIES COMPILE_FLAGS -fPIC)
    target_link_libraries(YARP_serversql LINK_PRIVATE ${CMAKE_DL_LIBS})
  endif ()

  # preserve yarpserver3 executable for a while
  add_executable(yarpserver3 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
  target_link_libraries(yarpserver3 YARP_serversql)

  add_executable(yarpserver ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
  target_link_libraries(yarpserver YARP_serversql)

  # set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_serversql)

  INSTALL(TARGETS YARP_serversql COMPONENT utilities DESTINATION lib)
  INSTALL(TARGETS yarpserver yarpserver3 COMPONENT utilities DESTINATION bin)

  ###############################################################
  ## Some extra test programs

  IF (TEST_BUILD)
    ADD_EXECUTABLE(server_peek src/server_peek.cpp)
    TARGET_LINK_LIBRARIES(server_peek YARP_serversql)
    ADD_EXECUTABLE(server_test src/server_test.cpp)
  ENDIF (TEST_BUILD)

endif ()
