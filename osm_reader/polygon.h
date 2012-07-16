#pragma once

#include<CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include<CGAL/Polygon_2.h>
#include<CGAL/create_offset_polygons_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

typedef K::FT                        FT ;
typedef K::Point_2                   Point ;
typedef CGAL::Polygon_2<K>           Polygon2 ;
typedef CGAL::Straight_skeleton_2<K> Ss ;

typedef std::tr1::shared_ptr<Polygon2> Polygon2Ptr ;
