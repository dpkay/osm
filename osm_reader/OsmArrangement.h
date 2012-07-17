#pragma once

#include <CGAL/Cartesian.h>
#include <CGAL/Quotient.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_polyline_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/IO/Arr_iostream.h>
#include <fstream>
#include "conversions.h"

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
    CGAL::insert(*cgal_arrangement_,
                 segments_to_insert_.begin(),
                 segments_to_insert_.end());
    segments_to_insert_.clear();
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
    Arrangement_2::Face_const_iterator fit;
    for (fit = cgal_arrangement_->faces_begin();
         fit != cgal_arrangement_->faces_end(); ++fit) {
      if(!fit->is_unbounded()) {
        Arrangement_2::Ccb_halfedge_const_circulator start = fit->outer_ccb();
        Arrangement_2::Ccb_halfedge_const_circulator current = start;
        int count = 0;
        do {
          count++;
          const Point_2& p = current->target()->point();
          out << "v " << CGAL::to_double(p.x()) << " "
                      << CGAL::to_double(p.y()) << " "
                      << "0.0" << std::endl;
        } while (++current != start);
        counts << count;
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
  typedef CGAL::Arr_segment_traits_2<Kernel>              Traits_2;
  typedef Traits_2::Point_2                               Point_2;
  typedef Traits_2::X_monotone_curve_2                    Segment_2;
  typedef CGAL::Arrangement_2<Traits_2>                   Arrangement_2;

  void print_ccb (Arrangement_2::Ccb_halfedge_const_circulator circ)
  {
    Arrangement_2::Ccb_halfedge_const_circulator curr = circ;
    std::cout << "(" << curr->source()->point() << ")";
    do {
      Arrangement_2::Halfedge_const_handle he = curr;
      std::cout << "   [" << he->curve() << "]   "
        << "(" << he->target()->point() << ")";
    } while (++curr != circ);
    std::cout << std::endl;
  }


  void print_face (Arrangement_2::Face_const_handle f)
  {
    // Print the outer boundary.
    if (f->is_unbounded())
      std::cerr << "Unbounded face. " << std::endl;
    else {
      std::cerr << "Outer boundary: ";
      print_ccb (f->outer_ccb());
    }

    // Print the boundary of each of the holes.
    Arrangement_2::Hole_const_iterator hi;
    int                                 index = 1;
    for (hi = f->holes_begin(); hi != f->holes_end(); ++hi, ++index) {
      std::cout << "    Hole #" << index << ": ";
      print_ccb (*hi);
    }

    // Print the isolated vertices.
    Arrangement_2::Isolated_vertex_const_iterator iv;
    for (iv = f->isolated_vertices_begin(), index = 1;
      iv != f->isolated_vertices_end(); ++iv, ++index)
    {
      std::cout << "    Isolated vertex #" << index << ": "
        << "(" << iv->point() << ")" << std::endl;
    }
  }
  


  std::tr1::shared_ptr<Arrangement_2> cgal_arrangement_;

  typedef std::map<uint, Point_2> OsmIdToCgalPointMap;
  std::map<uint, Point_2> vertices_;

  std::list<Segment_2> segments_to_insert_;

};

