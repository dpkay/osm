#include "osm_reader.h"
//#include "MeshWriter.h"
#include "OsmXmlFile.h"
#include "OsmArrangement.h"


osm_reader::osm_reader(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	//MeshWriter writer;
	//writer.write();
	//OsmGraph graph;
	
  //OsmArrangement arrangement;
 // OsmXmlFile file("sf_try1.osm");
	//if(!file.initialized()) {
	//	qFatal("unable to read osm file!");
	//}
 // qDebug() << "starting read";
	//if(!file.PopulateGraph(&arrangement)) {
 //   qCritical() << "populate graph FAILED";
 // }
 // qDebug() << "recomputing faces";
 // if(!arrangement.Flush()) {
 //   qCritical() << "flushing FAILED";
 // }
 // qDebug() << "done with faces";

  //arrangement.WriteToFile("sf_try1.cgal_arrangement");

  qDebug() << "loading arrangement";
  OsmArrangementPtr arrangement =
    OsmArrangement::FromFile("sf_try1.cgal_arrangement");
  arrangement->Flush();


  OsmPolygonSetPtr polygon_set(new OsmPolygonSet);
  qDebug() << "populating polygon set";
  arrangement->PopulatePolygonSet(polygon_set);
  qDebug() << "offsetting";
  polygon_set->RemoveDegeneratePolygons();
  polygon_set->Offset(1e-4f);
  qDebug() << "writing";
  polygon_set->WriteToObjFile("sf_try1_poly2.obj");

  //QFile epsFile("foo.eps");
  //epsFile.open(QIODevice::WriteOnly | QIODevice::Text);
  //QTextStream epsOut(&epsFile);
  //double factor = 30000;
  //foreach(Polygon2Ptr pp, graph.Faces()) {
  //  QVector<Point> pts;
  //  Polygon2::Vertex_iterator vit = pp->vertices_begin();
  //  epsOut << "0 0 0 setrgbcolor\n";
  //  epsOut << "newpath\n";
  //  epsOut << (int)((*vit)[0]*factor) << " "
  //         << (int)((*vit)[1]*factor) << " moveto\n";
  //  for (++vit; vit != pp->vertices_end(); ++vit) {
  //    epsOut << (int)((*vit)[0]*factor) << " "
  //           << (int)((*vit)[1]*factor) << " lineto\n";
  //    pts << *vit;
  //  }
  //  --vit;
  //  epsOut << "stroke\nnewpath\n"; 
  //  epsOut << (int)((*vit)[0]*factor) << " "
  //    << (int)((*vit)[1]*factor) << " moveto\n";
  //  epsOut << "1 0 0 setrgbcolor\n";
  //  vit = pp->vertices_begin();
  //  pts << *vit;
  //  epsOut << (int)((*vit)[0]*factor) << " "
  //         << (int)((*vit)[1]*factor) << " lineto\n";
  //  epsOut << "stroke\n";
  //  epsOut << "0 0 0 setrgbcolor\n";
  //  foreach (const Point& p, pts) {
  //    epsOut << "newpath\n";
  //    epsOut << p[0]*factor << " " << p[1]*factor
  //           << " 3 0 360 arc closepath\n";
  //    epsOut << "stroke\n";
  //  }
  //}
}

osm_reader::~osm_reader()
{

}
