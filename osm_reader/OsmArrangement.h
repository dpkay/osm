#pragma once

#include <CGAL/Cartesian.h>
#include <CGAL/Quotient.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_polyline_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/IO/Arr_iostream.h>
#include<CGAL/Polygon_2.h>
#include <fstream>
#include "conversions.h"
#include <CGAL/Polygon_2_algorithms.h>
#include "OsmPolygonSet.h"

class OsmArrangement;
typedef std::tr1::shared_ptr<OsmArrangement> OsmArrangementPtr;

class OsmArrangement
{
public:
  OsmArrangement(void):
      cgal_arrangement_(new Arrangement_2) {}

  ~OsmArrangement(void);

  static OsmArrangementPtr FromFile(const QString& filename) {
    OsmArrangementPtr ptr(new OsmArrangement(filename));
    return ptr;
  }

  void MakeAndAddVertex(uint osm_node_id, const Vector2& normalized_lat_lon) {
    vertices_[osm_node_id] = cgalFromEigen_2<Point_2>(normalized_lat_lon);
  }

  bool MakeAndAddEdge(uint osm_node_id_1, uint osm_node_id_2) {
    OsmIdToCgalPointMap::const_iterator vit1 = vertices_.find(osm_node_id_1);
    OsmIdToCgalPointMap::const_iterator vit2 = vertices_.find(osm_node_id_2);
    if(vit1 != vertices_.end() && vit2 != vertices_.end()) {
      const Point_2& p1 = vit1->second;
      const Point_2& p2 = vit2->second;
      Segment_2 segment(p1, p2);
      segments_to_insert_.push_back(segment);
    } else {
      qCritical() << "vertex not found during edge insertion!";
      return false;
    }
    return true;
  }

  bool Flush() {
    // compute arrangement
    if(segments_to_insert_.size() > 0) {
      CGAL::insert(*cgal_arrangement_,
                   segments_to_insert_.begin(),
                   segments_to_insert_.end());
      segments_to_insert_.clear();
    }
    return true;
  }

  bool PopulatePolygonSet(OsmPolygonSetPtr polygon_set) {
    // copy polygon set
    polygons_.clear();
    Arrangement_2::Face_const_iterator fit;
    for (fit = cgal_arrangement_->faces_begin();
         fit != cgal_arrangement_->faces_end(); ++fit)
    {
      if(!fit->is_unbounded()) {
        Arrangement_2::Ccb_halfedge_const_circulator start = fit->outer_ccb();
        Arrangement_2::Ccb_halfedge_const_circulator current = start;
        std::vector<Polygon_2::Point_2> vertices;
        do {
          vertices.push_back(Polygon_2::Point_2(CGAL::to_double(current->target()->point().x()),
                                                          CGAL::to_double(current->target()->point().y())));
        } while (++current != start);
        //polygons_.push_back(Polygon_2(vertices.begin(), vertices.end()));
        polygon_set->AddPolygon(vertices.begin(), vertices.end());
      }
    }

    qDebug() << "wrote " << polygons_.size() << " polygons";
    return true;
  }

  bool WriteToFile(QString filename) {
    std::ofstream out_file(filename.toStdString().c_str());
    out_file << *cgal_arrangement_;
    out_file.close();
    return true;
  }


  void WriteToObjFile(QString filename) {
    std::ofstream out(filename.toStdString().c_str());
    QList<int> counts;

    // write vertices
    for(size_t i=0; i<polygons_.size(); ++i) {
      qDebug() << CGAL::to_double(CGAL::polygon_area_2(polygons_[i].vertices_begin(),
                                       polygons_[i].vertices_end(), Inexact_Polygon_2::Traits()));

      Inexact_Polygon_2::Vertex_const_iterator vit;

      for(vit = polygons_[i].vertices_begin();
          vit != polygons_[i].vertices_end(); ++vit)
      {
        out << "v " << CGAL::to_double(vit->x()) << " "
          << CGAL::to_double(vit->y()) << " "
          << "0.0" << std::endl;
        counts << polygons_[i].size();
      }
    }

      // write faces
      int total = 1;
      for(size_t i=0; i<counts.size(); ++i) {
        out << "f ";
        for(int j=0; j<counts[i]; ++j) {
          out << total+j << " ";
        }
        out << std::endl;
        total += counts[i];
      }
   

    out.close();
  }

protected:
  OsmArrangement(const QString& filename):
       cgal_arrangement_(new Arrangement_2) {
         std::ifstream in_file(filename.toStdString().c_str());
         in_file >> *cgal_arrangement_;
         in_file.close();
   }


private:
  typedef CGAL::Quotient<CGAL::MP_Float>                  Number_type;
  typedef CGAL::Cartesian<Number_type>                    Kernel;
  //typedef CGAL::Exact_predicates_inexact_constructions_kernel   Kernel;
  typedef CGAL::Arr_segment_traits_2<Kernel>              Traits_2;
  typedef CGAL::Polygon_2<Kernel>                         Polygon_2;
  typedef CGAL::Polygon_2<CGAL::Exact_predicates_inexact_constructions_kernel>                         Inexact_Polygon_2;

  typedef Traits_2::Point_2                               Point_2;
  typedef Traits_2::X_monotone_curve_2                    Segment_2;
  typedef CGAL::Arrangement_2<Traits_2>                   Arrangement_2;
  


  std::tr1::shared_ptr<Arrangement_2> cgal_arrangement_;

  typedef std::map<uint, Point_2> OsmIdToCgalPointMap;
  std::map<uint, Point_2> vertices_;

  std::list<Segment_2> segments_to_insert_;

  std::vector<Inexact_Polygon_2> polygons_;
};

