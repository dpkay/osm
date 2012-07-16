#pragma once

#include <miro\GeometryData.h>
#include <miro\GeometryNodes/GeometryNode.h>
#include "OsmGraph.h"

class OsmMeshNode : public GeometryNode
{
public:
  OsmMeshNode() {
    osm_graph_ = NULL;
  }

  ~OsmMeshNode(void);

  virtual const char* typeName() {
    return "OsmMesh";
  }

  virtual bool recompute() {
    // create empty polymesh
    QSharedPointer<GeometryData> data = GeometryData::createPolymesh();
    PolyMesh * polyMesh = data->polymeshes()[0].data();

    if(osm_graph_ = NULL) {
      qWarning() << typeName() << name() << "has no osm graph attached!";
      return false;
    }

    // append geometry
    float vertices[] = {
      -0.5, -0.5, -0.5,
      +0.5, -0.5, -0.5,
      -0.5, +0.5, -0.5,
      +0.5, +0.5, -0.5
    };
    unsigned int indices[] = {
      0, 1, 3, 2,
    };

    polyMesh->setVerticesFromCArray(vertices, 12);
    int submeshId = polyMesh->addSubmeshWithIndicesFromCArray(QuadPatches, indices, 4);
    
    setData(data);
    return true;
  }

  const OsmGraph* osm_graph() const { return osm_graph_; }
  void set_osm_graph(const OsmGraph* val) { osm_graph_ = val; }

private:
  const OsmGraph* osm_graph_;

};

