#include "StdAfx.h"
#include "OsmPolygonSet.h"


void OsmPolygonSet::Offset( double offset )
{
  PolygonVectorPtr_2 new_polygons(new PolygonVector_2);
  for(size_t i=0; i<cgal_polygons_->size(); ++i) {
    qDebug() << "offsetting" << i << " " << CGAL::to_double(CGAL::polygon_area_2((*cgal_polygons_)[i].vertices_begin(),
      (*cgal_polygons_)[i].vertices_end(), Polygon_2::Traits()));
    std::vector<boost::shared_ptr<Polygon_2> > inner_offset_polygons =
      CGAL::create_interior_skeleton_and_offset_polygons_2(
      offset, (*cgal_polygons_)[i]);
    
    for(size_t j=0; j<inner_offset_polygons.size(); ++j) {
      new_polygons->push_back(*inner_offset_polygons[j]);
    }
  }
  cgal_polygons_ = new_polygons;
}

#include <Eigen/SVD>

void OsmPolygonSet::RemoveDegeneratePolygons()
{
  PolygonVectorPtr_2 polygons_to_keep(new PolygonVector_2);
  PolygonVector_2::const_iterator it;
  int count = 0;
  for(it = cgal_polygons_->begin(); it<cgal_polygons_->end(); ++it) {
    
    // test for too eccentric polygons
    Eigen::MatrixXf points = Eigen::MatrixXf::Zero(it->size(), 2);    
    Polygon_2::Vertex_const_iterator vit;
    int vcount = 0;
    for(vit = it->vertices_begin(); vit != it->vertices_end(); ++vit) {
      points(vcount, 0) = vit->x();
      points(vcount, 1) = vit->y();
      vcount++;
    }
    const Eigen::RowVector2f mean = points.colwise().sum() / it->size();  
    Eigen::RowVectorXf sv = (points.rowwise() - mean).jacobiSvd().singularValues();
    double eccentricity = sv[0]/sv[1];
    if(eccentricity > 4) {
      continue;
    }

    // test for too acute angles
    vcount = 0;
    Eigen::Vector2f pre_previous;
    Eigen::Vector2f previous;
    Eigen::Vector2f current;
    bool too_acute = false;
    for(vit = it->vertices_begin(); vit != it->vertices_end(); ++vit) {
      pre_previous = previous;
      previous = current;
      current = Eigen::Vector2f(vit->x(), vit->y());
      vcount++;
      if(vcount > 2) {
        Eigen::Vector2f a = (previous-current).normalized();
        Eigen::Vector2f b = (previous-pre_previous).normalized();
        if(a.dot(b) > 0.985) {
          too_acute = true;
          break;
        }
      }
    }
    if(too_acute) {
      continue;
    }

    polygons_to_keep->push_back(*it);
    count++; 
  }
  qDebug() << "before" << cgal_polygons_->size() << "after" << polygons_to_keep->size();
  cgal_polygons_ = polygons_to_keep;
}
