#pragma once

#include "polygon.h"

class OsmEdge
{
public:
	OsmEdge(uint osmNodeIdA, uint osmNodeIdB) {
		_osmNodeIds.first = osmNodeIdA;
		_osmNodeIds.second = osmNodeIdB;
	}

	void setStreetName(const std::string& val) { _streetName = val; }
	void setType(const std::string& val) { _type = val; }

	const std::string& streetName() const { return _streetName; }
	const std::string& type() const { return _type; }

private:
	std::pair<uint, uint> _osmNodeIds;
	std::string _streetName;
	
	std::string _type;
	
};

//
//class OsmFace
//{
//	//void addOsmNodeIds(uint osmNodeId) { _osmNodeIds.push_back(osmNodeId); }
//	//const std::vector<uint>& osmNodeIds() const { return _osmNodeIds; }
//public:
//  const std::vector<Vector2>& lat_lon_coordinates() const { return lat_lon_coordinates_; }
//  void add_lat_lon_coordinate(const Vector2& val) { lat_lon_coordinates_.push_back(val); }
//
//private:
//	//std::vector<uint> _osmNodeIds;
//  std::vector<Vector2> lat_lon_coordinates_;
//};

//typedef std::tr1::shared_ptr<OsmFace> OsmFacePtr;

class OsmGraph
{
 public:
	OsmGraph(void);
	~OsmGraph(void);

	void MakeAndAddVertex(uint osm_node_id, Vector2 normalized_lat_lon) {
    BglVertexType vertex(osm_node_id, normalized_lat_lon);
    BglGraph::vertex_descriptor descriptor =
      boost::add_vertex(vertex, bgl_graph_);
    osm_id_to_bgl_vertex_[osm_node_id] = descriptor;
  }

	bool MakeAndAddEdge(uint osm_node_id_1, uint osm_node_id_2) {
    OsmIdToBglVertexIterator it1 = osm_id_to_bgl_vertex_.find(osm_node_id_1);
    OsmIdToBglVertexIterator it2 = osm_id_to_bgl_vertex_.find(osm_node_id_2);
    if (it1 == osm_id_to_bgl_vertex_.end() ||
        it2 == osm_id_to_bgl_vertex_.end()) {
      qCritical() << "one of the two nodes couldn't be found:" <<
        osm_node_id_1 << osm_node_id_2;
      return NULL;
    }

    boost::add_edge(it1->second, it2->second, /*BglEdgeType(), */bgl_graph_);
    return true;
  }

  void AddFace(Polygon2Ptr face) {
    faces_.push_back(face);
  }
	
  bool Flush() {
    std::vector<std::vector<boost::graph_traits<BglGraph>::edge_descriptor> > embedding(
      boost::num_vertices(bgl_graph_));
    bool isPlanar = boost::boyer_myrvold_planarity_test(
      boost::boyer_myrvold_params::graph = bgl_graph_,
      boost::boyer_myrvold_params::embedding = &embedding[0]);
    VertexOutputVisitor visitor(this, &bgl_graph_);
    if(!isPlanar) {
      qCritical("graph is not planar!");
      return false;
    }

    // Initialize the interior edge index
    boost::property_map<BglGraph, boost::edge_index_t>::type e_index =
      boost::get(boost::edge_index, bgl_graph_);
    BglGraph::edges_size_type edge_count = 0;
    BglGraph::edge_iterator ei, ei_end;
    for(boost::tie(ei, ei_end) = boost::edges(bgl_graph_); ei != ei_end; ++ei)
      boost::put(e_index, *ei, edge_count++);

    //std::map<uint, uint> x;
    boost::planar_face_traversal(bgl_graph_, &embedding[0], visitor);
    return true;
  }
	const std::vector<Polygon2Ptr>& Faces() const { return faces_; }

 private:
  struct BglVertexType {
    BglVertexType() {
      osm_node_id_ = -1;
    }

    BglVertexType(int osm_node_id, const Vector2& normalized_lat_lon) {
      osm_node_id_ = osm_node_id;
      normalized_lat_lon_ = normalized_lat_lon;
    }

    int osm_node_id_;
    Vector2 normalized_lat_lon_;
  };

  struct BglEdgeType {
    //BglEdgeType(int osm_node_id, int lat_lon) {
    //  osm_node_id_ = osm_node_id;
    //  lat_lon_ = lat_lon;
    //}

    //int osm_node_id_;
    //Eigen::Vector2f lat_lon_;
  };

  typedef  boost::adjacency_list<
		  boost::vecS, // store edges as vector
      boost::vecS, // store vertices as vector
      boost::undirectedS, // choose undirected graph
      //boost::property<boost::vertex_index_t, int>,
      //boost::property<boost::edge_index_t, int> > BglGraph;
      BglVertexType,
      boost::property<boost::edge_index_t, int>> BglGraph;
		 /* boost::property<boost::vertex_index_t, uint>,
		  boost::property<boost::edge_index_t, uint> >*/// BglGraph;

	BglGraph bgl_graph_;
  typedef std::map<uint, BglGraph::vertex_descriptor> OsmIdToBglVertexMap;
  typedef OsmIdToBglVertexMap::const_iterator OsmIdToBglVertexIterator;
  OsmIdToBglVertexMap osm_id_to_bgl_vertex_;


  struct VertexOutputVisitor : public boost::planar_face_traversal_visitor
  {
    VertexOutputVisitor(OsmGraph* osmGraph, const BglGraph* bglGraph) {
      osm_graph_ = osmGraph;
      bgl_graph_ = bglGraph;
      current_face_ = NULL;
    }

    void begin_face() {
      //qDebug() << "begin face";
      current_face_.reset(new Polygon2);
    }

    void end_face() {
      qDebug() << "end face with area" << current_face_->area();
      Polygon2::Vertex_const_iterator vit;
      for(vit = current_face_->vertices_begin();
          vit != current_face_->vertices_end(); ++vit) {
        //qDebug() << vit->x() << vit->y();
//         std::cerr << *vit << std::endl;
      }
      osm_graph_->AddFace(current_face_);
      current_face_ = NULL;
    }

    void next_vertex(const BglGraph::vertex_descriptor& vd) {
      const BglVertexType& vertex = (*bgl_graph_)[vd];
      //qDebug() << "next vertex" << vertex.normalized_lat_lon_[0] << vertex.normalized_lat_lon_[1];
      //current_face_->add_lat_lon_coordinate(vertex.lat_lon_);
      current_face_->push_back(Point(vertex.normalized_lat_lon_[0], vertex.normalized_lat_lon_[1]));
    }

  private:
    const BglGraph* bgl_graph_;
    OsmGraph* osm_graph_;
    Polygon2Ptr current_face_;

  };

  //std::map<
	std::vector<Polygon2Ptr> faces_;
};
