#  Yarp Application Builder
#  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
# 
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
 

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

FILE(GLOB folder_source src/*.cpp)
FILE(GLOB folder_header include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

ADD_DEFINITIONS(${GtkMM_C_FLAGS})

if(UNIX)
    if (GtkMM_VERSION_MAJOR GREATER 2 OR GtkMM_VERSION_MAJOR EQUAL 2)
        if (GtkMM_VERSION_MINOR LESS 22)
            add_definitions(-DGTKMM_LESS_2_22)
         endif()
    endif()

    add_definitions(-Wno-trigraphs)

endif(UNIX)

if(MSVC)
    add_definitions(/vd2)
endif(MSVC)

INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${TinyXML_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GtkMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${GooCanvasMM_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(../libymanager/include)

link_directories(${GooCanvas_LIBRARY_DIRS})

# GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES

## FIXME 2.4 Move the check in conf/YarpSystemCheck.cmake later
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wno-mismatched-tags" CXX_HAS_WNO_MISMATCHED_TAGS)
if(CXX_HAS_WNO_MISMATCHED_TAGS)
   set_source_files_properties(${folder_source}
                               PROPERTIES COMPILE_FLAGS "-Wno-mismatched-tags")
endif(CXX_HAS_WNO_MISMATCHED_TAGS)

ADD_EXECUTABLE(gyarpbuilder ${folder_source} ${folder_header})
if(YARP_USE_GTK2)
    target_link_libraries(gyarpbuilder YARP_init
                                       YARP_OS
                                       YARP_manager
                                       ${GTK2_GTK_LIBRARY}
                                       ${GTK2_GDK_LIBRARY}
                                       ${GTK2_GLIB_LIBRARY}
                                       ${GTK2_GTKMM_LIBRARY}
                                       ${GTK2_GLIBMM_LIBRARY}
                                       ${GTK2_GDKMM_LIBRARY}
                                       ${GTK2_ATKMM_LIBRARY}
                                       ${GTK2_SIGC++_LIBRARY}
                                       ${GooCanvasMM_LIBRARIES})
else(YARP_USE_GTK2)
    target_link_libraries(gyarpbuilder YARP_init
                                       YARP_OS
                                       YARP_manager
                                       ${GtkMM_LIBRARIES}
                                       ${GooCanvasMM_LIBRARIES})
endif(YARP_USE_GTK2)

INSTALL(TARGETS gyarpbuilder COMPONENT utilities DESTINATION bin)



