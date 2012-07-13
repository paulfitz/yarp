# GtkDataboxMM
project(gtkdataboxmm)

include_directories(${GTKMM_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/gtkdataboxmm/gtkdatabox/)

set(gtkdataboxmm_SRCS
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/points.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/databox.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/graph.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/cross_simple.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/markers.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/ruler.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/grid.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/xyc_graph.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/wrap_init.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/scale.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/bars.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/init.cc
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/lines.cc
)

set(gtkdataboxmm_HDRS
    gtkdataboxmm/gtkdatabox/gtkdataboxmmconfig.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/wrap_init.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/cross_simple.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/xyc_graph.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/init.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/bars.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/scale.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/grid.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/points.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/markers.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/lines.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/cross_simple_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/lines_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/points_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/bars_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/markers_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/ruler_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/databox_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/xyc_graph_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/graph_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/grid_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/private/scale_p.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/databox.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/graph.h
    gtkdataboxmm/gtkdatabox/gtkdataboxmm/ruler.h
)

add_library(gtkdataboxmm STATIC ${gtkdataboxmm_SRCS})

set(GTKDATABOXMM_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/gtkdataboxmm/gtkdatabox" CACHE PATH "GtkDataboxMM include directory" FORCE)
set(GTKDATABOXMM_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/lib ${GTKDATABOX_LIBRARY_DIRS} ${GTKMM_LIBRARY_DIRS}" CACHE PATH "GtkDataboxMM library directory" FORCE)
set(GTKDATABOXMM_LIBRARIES "gtkdataboxmm;${GTKDATABOX_LIBRARIES};${GTKMM_LIBRARIES}" CACHE STRING "GtkDataboxMM libraries" FORCE)
mark_as_advanced(GTKDATABOXMM_INCLUDE_DIRS GTKDATABOXMM_LIBRARY_DIRS GTKDATABOXMM_LIBRARIES)