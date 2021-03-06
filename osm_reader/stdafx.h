#define NOMINMAX

#include <GL/glew.h>
#include <QtCore/QVariant>
#include <QGLWidget>
#include <miro/miro.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/ref.hpp>
#include <Eigen/Dense>
#include <utility>
#include <string>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include "types.h"



#include<CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include<CGAL/Polygon_2.h>
#include<CGAL/create_offset_polygons_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Quotient.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_polyline_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <QDomDocument>
#include <CGAL/IO/Arr_iostream.h>
#include <fstream>
