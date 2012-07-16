#pragma once

#include <string>
#include <QDomDocument>
#include "OsmGraph.h"

class OsmXmlFile
{
 public:
	OsmXmlFile(const QString& path) :
		file_(path),
		dom_document_("mydocument")
	{
		if(!file_.open(QIODevice::ReadOnly)) {
			file_.close();
			return;
		}
		if(!dom_document_.setContent(&file_)) {
			file_.close();
			return;
		}
		root_element_ = dom_document_.documentElement();
		if(root_element_.nodeName() != "osm") {
			file_.close();
			return;
		}
		initialized_ = true;
	}

	~OsmXmlFile(void);

	bool ParseOsmNode(OsmGraph* graph, const QDomElement& element) {
    //qDebug() << "parsing node" << element.attribute("id");
		if (!element.hasAttribute("id") || 
		    !element.hasAttribute("lat") ||
		    !element.hasAttribute("lon")) {
			qCritical("node attribute needs attributes: id, lat, lon");
	    return false;
		}

    int id = element.attribute("id").toUInt();
    Vector2 lat_lon(
      element.attribute("lat").toDouble(),
      element.attribute("lon").toDouble());
    lat_lon -= min_lat_lon_;

    std::cerr << id << lat_lon;
    //qDebug() << id << lat_lon;

		graph->MakeAndAddVertex(id, lat_lon);
		return true;
	}

	bool ParseOsmWay(OsmGraph* graph, const QDomElement& element) {
	//	qDebug() << "parsing way";
    QVector<uint> nodes;
    QString highway_type;

    QDomNode node = element.firstChild();
    while (!node.isNull()) {
      QDomElement element = node.toElement();
      if (!element.isNull()) {
        if (element.tagName() == "tag") {
          if(element.attribute("k") == "highway") {
            highway_type = element.attribute("v");
          }
        } else
        if (element.tagName() == "nd") {
          nodes << element.attribute("ref").toUInt();
        } 
      }
      node = node.nextSibling();
    }

    if(!highway_type.isNull())
      qDebug() << qPrintable(highway_type);
    if(highway_type == "residential") {
      for(size_t i=0; i<nodes.size()-1; ++i) {
        graph->MakeAndAddEdge(nodes[i], nodes[i+1]);
      }
    }
    //graph->MakeAndAddEdge(nodes[1], nodes[0]);
    //graph->MakeAndAddEdge(nodes[nodes.size()-1], nodes[0]);
    //qDebug() << highway_type << nodes.size();
		return true;
	}

	bool PopulateGraph(OsmGraph* graph) {
		QDomNode node = root_element_.firstChild();
    
    // get bounds
    QDomElement element = node.toElement();
    if(element.isNull() || element.tagName() != "bounds") {
      qCritical("no bounds available!");
      return false;
    }
    min_lat_lon_ = Vector2(
      element.attribute("minlat").toDouble(),
      element.attribute("minlon").toDouble());
    node = node.nextSibling();

    // get all other elements
		while (!node.isNull()) {
      element = node.toElement();
      if (!element.isNull()) {
				if (element.tagName() == "node") {
					if (!ParseOsmNode(graph, element)) {
						return false;
					}
				} else
				if(element.tagName() == "way") {
					if(!ParseOsmWay(graph, element)) {
						return false;
					}
				}
			}
			node = node.nextSibling();
		}
    initialized_ = true;
		return true;
	}

	bool initialized() const { return initialized_; }

 private:
	QFile file_;
	QDomDocument dom_document_;
	QDomElement root_element_;
  Eigen::Vector2d min_lat_lon_;

	bool initialized_;

};

