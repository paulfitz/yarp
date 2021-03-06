# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

set(YARP_HAS_MATH_LIB FALSE)
if (CREATE_LIB_MATH)
    set(YARP_HAS_MATH_LIB TRUE)
endif(CREATE_LIB_MATH)

if (YARP_HAS_MATH_LIB)

  project(YARP_math)

  file(GLOB folder_source src/*.cpp)
  file(GLOB_RECURSE folder_header include/*.h)
  source_group("Source Files" FILES ${folder_source})
  source_group("Header Files" FILES ${folder_header})

  set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include)
  get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})

  if(YARP_USE_ATLAS)
     set(MATH_LIB_EXTRAS ${GSL_LIBRARY} ${ATLAS_CBLAS_LIBRARY} ${ATLAS_ATLAS_LIBRARY})
  else(YARP_USE_ATLAS)
     set(MATH_LIB_EXTRAS ${GSL_LIBRARIES})
  endif(YARP_USE_ATLAS)

  include_directories(${GSL_INCLUDE_DIRS})

  add_library(YARP_math ${folder_source} ${folder_header})
  target_link_libraries(YARP_math YARP_sig YARP_OS ${MATH_LIB_EXTRAS})
  install(TARGETS YARP_math EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_math)

  set_property(TARGET YARP_math PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

  install(DIRECTORY include/yarp/math COMPONENT development DESTINATION include/yarp FILES_MATCHING PATTERN "*.h" PATTERN ".*" EXCLUDE)


endif(YARP_HAS_MATH_LIB)

set_property(GLOBAL PROPERTY YARP_HAS_MATH_LIB ${YARP_HAS_MATH_LIB})
