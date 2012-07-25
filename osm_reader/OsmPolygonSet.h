#pragma once

class OsmPolygonSet;
typedef std::tr1::shared_ptr<OsmPolygonSet> OsmPolygonSetPtr;


class OsmPolygonSet
{
public:
  OsmPolygonSet(void) : cgal_polygons_(new PolygonVector_2) {}
  ~OsmPolygonSet(void) {}

  template <class PointIterator>
  void AddPolygon(PointIterator begin, PointIterator end) {
    std::vector<Point_2> vertices;
    //std::copy(begin, end, vertices.begin());
    for(PointIterator it = begin; it != end; ++it) {
      vertices.push_back(Point_2(CGAL::to_double(it->x()),
                                 CGAL::to_double(it->y())));
    }
    //cgal_polygons_.push_back(Polygon_2(begin, end));
    cgal_polygons_->push_back(Polygon_2(vertices.begin(), vertices.end()));
    
  }


  void RemoveDegeneratePolygons();

  void Offset(double offset);


  void WriteToObjFile(QString filename) {
    std::ofstream out(filename.toStdString().c_str());
    QList<int> counts;

    // write vertices
    for(size_t i=0; i<cgal_polygons_->size(); ++i) {
      qDebug() << CGAL::to_double(CGAL::polygon_area_2((*cgal_polygons_)[i].vertices_begin(),
        (*cgal_polygons_)[i].vertices_end(), Polygon_2::Traits()));

      Polygon_2::Vertex_const_iterator vit;
      for(vit = (*cgal_polygons_)[i].vertices_begin();
          vit != (*cgal_polygons_)[i].vertices_end(); ++vit)
      {
        out << "v " << CGAL::to_double(vit->x()) << " "
          << CGAL::to_double(vit->y()) << " "
          << "0.0" << std::endl;
      }
      counts << (*cgal_polygons_)[i].size();
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


  typedef CGAL::Exact_predicates_inexact_constructions_kernel    Kernel;
  typedef CGAL::Polygon_2<Kernel>                                Polygon_2;
  typedef Polygon_2::Traits                                Traits_2;
  typedef Traits_2::Point_2   Point_2;

  typedef std::vector<Polygon_2> PolygonVector_2;
  typedef std::tr1::shared_ptr<PolygonVector_2> PolygonVectorPtr_2;
  PolygonVectorPtr_2 cgal_polygons_;

};

