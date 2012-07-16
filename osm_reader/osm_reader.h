#ifndef OSM_READER_H
#define OSM_READER_H

#include <QtGui/QMainWindow>
#include "ui_osm_reader.h"

class osm_reader : public QMainWindow
{
	Q_OBJECT

public:
	osm_reader(QWidget *parent = 0, Qt::WFlags flags = 0);
	~osm_reader();

private:
	Ui::osm_readerClass ui;
};

#endif // OSM_READER_H
