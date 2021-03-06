# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(YARP_COMPILE_TESTS "Compile YARP tests" TRUE)

if (YARP_COMPILE_TESTS)

  add_executable(testModule ../libYARP_OS/harness/testModule/module.cpp)
  target_link_libraries(testModule YARP_init YARP_OS)
  
  #### temporary variable for debug
  set(ENABLE_YARPRUNTESTS FALSE CACHE BOOL "Enable yarprun tests")
  if (ENABLE_YARPRUNTESTS)
	add_definitions(-DYARPRUN_TEST)
  endif()
  mark_as_advanced(ENABLE_YARPRUNTESTS)
  
  set(targets OS sig dev)
  get_property(YARP_HAS_MATH_LIB GLOBAL PROPERTY YARP_HAS_MATH_LIB)
  if (YARP_HAS_MATH_LIB)
    set(targets ${targets} math)
  endif (YARP_HAS_MATH_LIB)

  foreach(test_family ${targets})
    file(GLOB harness_code ${CMAKE_SOURCE_DIR}/src/libYARP_${test_family}/harness/*.cpp ${CMAKE_SOURCE_DIR}/src/libYARP_${test_family}/harness/*.h)
    get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_${test_family} 
                 PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_TREE_INCLUDE_DIRS})
    if (YARP_HAS_ACE)
      include_directories(${ACE_INCLUDE_DIRS})
    else (YARP_HAS_ACE)
    endif (YARP_HAS_ACE)
    set(EXE harness_${test_family})
    string(TOLOWER ${EXE} EXE)
    add_executable(${EXE} ${harness_code})
    target_link_libraries(${EXE} YARP_init YARP_${test_family})
    target_link_libraries(${EXE} ${ACE_LIBRARIES})
    # added by Lorenzo, to fix windows problem. I assume we test RELEASE 
    # builds.
    GET_TARGET_PROPERTY(EXELOC ${EXE} LOCATION_RELEASE)
    #message(STATUS "Exeloc: ${EXELOC}")
    if (NOT YARP_HAS_ACE)
      # Without ACE, we don't currently have UDP
      list(REMOVE_ITEM harness_code ${CMAKE_SOURCE_DIR}/src/libYARP_${test_family}/harness/DgramTwoWayStreamTest.cpp)
    endif ()
    foreach(test ${harness_code})
      get_filename_component(XN ${test} NAME_WE)
  	#message(STATUS ${XN})
      add_test("${test_family}::${XN}" ${EXELOC} regression ${XN})
    endforeach()
  endforeach()

  # Add in hardware specific tests, if requested
  option(CREATE_PLUGIN_TESTS "Compile plugin tests" FALSE)
  mark_as_advanced(CREATE_PLUGIN_TESTS)
  if (CREATE_PLUGIN_TESTS)
    # add in our tests
    file(GLOB inis ../libYARP_dev/harness/*.ini)
    foreach(X ${inis})
      get_filename_component(XN ${X} NAME_WE)
      get_filename_component(XINI ${X} ABSOLUTE)
      # need to cache examples for reference
      set(DOC_DIR "${CMAKE_SOURCE_DIR}/src/doc/device_invocation")
      set(XDOX ${DOC_DIR}/${XN}.dox)
      add_test(device::${XN} ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR}/src/libYARP_dev/harness ${EXECUTABLE_OUTPUT_PATH}/harness_dev --file "${XINI}" --doc "${XDOX}")
    endforeach(X ${inis})
  endif (CREATE_PLUGIN_TESTS)

endif (YARP_COMPILE_TESTS)

# touch 1
